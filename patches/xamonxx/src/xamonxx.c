/*
 * xamonxx naga8gen5 — per-mode runtime tuning for Turnip on Adreno 829 (iQOO 15R).
 * Copied into src/vulkan/runtime/ by patches/xamonxx/0002_xamonxx_runtime.py.
 *
 * What lives here
 *   - mode table (cool60 / monster90 / monster120) selected at compile time
 *     (-DXAMONXX_MODE_ID), by mode.txt (debug zip only) or by env vars;
 *   - FPS limiter: called once per present, sleeps to a fixed cadence;
 *   - thermal guard (Monster modes): if the hottest CPU/GPU sensor stays above
 *     95 °C the limiter steps down 15 fps (never below the mode minimum), and
 *     steps back up after 30 s below 85 °C. Less work = less heat; it never
 *     touches thermal limits, it only renders fewer frames;
 *   - ADPF (Android Performance Hint) session: a *hint* to the OS CPU scheduler,
 *     loaded with dlopen so the driver gains no new NEEDED library;
 *   - evaluation log (debug zip only): one CSV row per second written by a
 *     background thread (fps, frame times, GPU busy/clock, CPU clock, temps).
 *
 * Nothing here touches sysfs for writing, thermal limits or CPU governors.
 * Every failure path degrades to "feature off" and the driver keeps running.
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "xamonxx.h"

#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "util/log.h"

#ifndef XAMONXX_MODE_ID
#define XAMONXX_MODE_ID 0
#endif
#ifndef XAMONXX_DEBUG
#define XAMONXX_DEBUG 0
#endif
#ifndef XAMONXX_VERSION
#define XAMONXX_VERSION "dev"
#endif

#define XLOG_PREFIX "xamonxx naga8gen5: "

/* ---- mode table ------------------------------------------------------------ */

struct xamonxx_mode {
   const char *name;
   int gpu_floor, fps_limit, target_fps, fps_min, guard, adpf;
};

/* V0.4 (tuned from the owner's V0.3 debug logs, 2026-11-24):
 *  - cool60: no pacing limiter (the game's Target FPS 60 + V-Sync already pace it;
 *    a second pacer at 60 caused 33 ms V-Sync misses), GPU floor 40 -> 60 %, plain
 *    ADPF session (the power-efficient hint let CPU clocks sag to 0.5-0.9 GHz).
 *  - monster90/120: thermal guard (GPU hit 104 °C, CPU 109 °C in 2 minutes).
 * V0.5 (V0.4 logs):
 *  - cool60: safety cap 63 fps. With the game locked at 60 frames arrive every
 *    >= 16.67 ms > 15.87 ms, so the cap never sleeps (no double pacing); with the
 *    game set to Unlimited it stops the 93 fps / GPU 98 °C case seen in the log.
 *  - monster90: guard minimum 60 -> 75 (the guard no longer reacts to CPU spikes). */
static const struct xamonxx_mode xamonxx_modes[] = {
   /*            name          floor limit target min guard adpf */
   /* id 0 */ { "base",         0,    0,    60,    0,  0,    0 },
   /* id 1 */ { "cool60",       60,   63,   60,   63,  0,    1 },
   /* id 2 */ { "monster90",    1,    90,   90,   75,  1,    1 },
   /* id 3 */ { "monster120",   1,    120,  120,  90,  1,    1 },
};
#define XAMONXX_NUM_MODES ((int)(sizeof(xamonxx_modes) / sizeof(xamonxx_modes[0])))

static struct xamonxx_config cfg;
static pthread_once_t cfg_once = PTHREAD_ONCE_INIT;
static char log_dir[512];          /* empty = no writable folder found */
static char mode_source[32] = "compiled";
static atomic_int cur_limit;       /* limiter value in use (thermal guard may lower it) */
static atomic_int guard_steps;     /* how many times the guard changed the limit */

static uint64_t
now_ns(void)
{
   struct timespec ts;
   clock_gettime(CLOCK_MONOTONIC, &ts);
   return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

static void
apply_mode(const struct xamonxx_mode *m)
{
   cfg.mode = m->name;
   cfg.gpu_floor = m->gpu_floor;
   cfg.fps_limit = m->fps_limit;
   cfg.target_fps = m->target_fps;
   cfg.fps_min = m->fps_min;
   cfg.guard = m->guard;
   cfg.adpf = m->adpf;
}

static const struct xamonxx_mode *
find_mode(const char *name)
{
   for (int i = 0; i < XAMONXX_NUM_MODES; i++) {
      if (!strcmp(name, xamonxx_modes[i].name))
         return &xamonxx_modes[i];
   }
   return NULL;
}

static int
clamp_int(int v, int lo, int hi)
{
   return v < lo ? lo : (v > hi ? hi : v);
}

#if XAMONXX_DEBUG
/* mkdir -p for a short absolute path. Returns true if the dir is writable. */
static bool
make_writable_dir(const char *path)
{
   char tmp[512];
   snprintf(tmp, sizeof(tmp), "%s", path);
   for (char *p = tmp + 1; *p; p++) {
      if (*p == '/') {
         *p = '\0';
         mkdir(tmp, 0775);
         *p = '/';
      }
   }
   mkdir(tmp, 0775);
   return access(tmp, W_OK | X_OK) == 0;
}

/* Package name of the host app, from /proc/self/cmdline or a /data/... path in it. */
static bool
host_package(char *out, size_t out_size)
{
   char buf[1024];
   int fd = open("/proc/self/cmdline", O_RDONLY | O_CLOEXEC);
   if (fd < 0)
      return false;
   ssize_t n = read(fd, buf, sizeof(buf) - 1);
   close(fd);
   if (n <= 0)
      return false;
   buf[n] = '\0';
   for (ssize_t i = 0; i < n; i++) {
      if (buf[i] == '\0')
         buf[i] = ' ';
   }

   const char *prefixes[] = { "/data/data/", "/data/user/0/" };
   for (unsigned i = 0; i < 2; i++) {
      const char *p = strstr(buf, prefixes[i]);
      if (p) {
         p += strlen(prefixes[i]);
         size_t len = strcspn(p, "/ ");
         if (len > 0 && len < out_size) {
            memcpy(out, p, len);
            out[len] = '\0';
            return true;
         }
      }
   }

   /* ART app processes: argv[0] is the package name (may carry ":process"). */
   size_t len = strcspn(buf, " :");
   if (len > 0 && len < out_size && memchr(buf, '.', len) && !memchr(buf, '/', len)) {
      memcpy(out, buf, len);
      out[len] = '\0';
      return true;
   }
   return false;
}

static void
find_log_dir(void)
{
   const char *candidates[] = {
      "/sdcard/Download/xamonxx",
      "/storage/emulated/0/Download/xamonxx",
   };
   for (unsigned i = 0; i < 2; i++) {
      if (make_writable_dir(candidates[i])) {
         snprintf(log_dir, sizeof(log_dir), "%s", candidates[i]);
         return;
      }
   }

   char pkg[256], path[512];
   if (host_package(pkg, sizeof(pkg))) {
      snprintf(path, sizeof(path), "/data/data/%s/files/xamonxx", pkg);
      if (make_writable_dir(path)) {
         snprintf(log_dir, sizeof(log_dir), "%s", path);
         return;
      }
   }

   const char *envs[] = { "TMPDIR", "HOME" };
   for (unsigned i = 0; i < 2; i++) {
      const char *base = getenv(envs[i]);
      if (base && *base == '/') {
         snprintf(path, sizeof(path), "%s/xamonxx", base);
         if (make_writable_dir(path)) {
            snprintf(log_dir, sizeof(log_dir), "%s", path);
            return;
         }
      }
   }
   log_dir[0] = '\0';
}

/* Reads mode.txt (first non-comment word). Creates it with "cool60" if missing. */
static void
load_mode_file(void)
{
   if (!log_dir[0])
      return;

   char path[600];
   snprintf(path, sizeof(path), "%s/mode.txt", log_dir);
   FILE *f = fopen(path, "r");
   if (!f) {
      f = fopen(path, "w");
      if (f) {
         fputs("cool60\n"
               "# xamonxx naga8gen5 debug driver\n"
               "# Ganti baris pertama dengan salah satu: cool60 | monster90 | monster120\n"
               "# lalu tutup dan buka ulang game. Log CSV ada di folder ini.\n", f);
         fclose(f);
      }
      apply_mode(&xamonxx_modes[1]);
      snprintf(mode_source, sizeof(mode_source), "mode.txt(created)");
      return;
   }

   char line[128];
   while (fgets(line, sizeof(line), f)) {
      char *p = line;
      while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
         p++;
      if (!*p || *p == '#')
         continue;
      p[strcspn(p, " \t\r\n#")] = '\0';
      const struct xamonxx_mode *m = find_mode(p);
      if (m) {
         apply_mode(m);
         snprintf(mode_source, sizeof(mode_source), "mode.txt");
      } else {
         mesa_logw(XLOG_PREFIX "mode.txt: unknown mode '%s', using %s", p, cfg.mode);
         snprintf(mode_source, sizeof(mode_source), "mode.txt(invalid)");
      }
      break;
   }
   fclose(f);
}
#endif /* XAMONXX_DEBUG */

static void
init_config(void)
{
   int id = clamp_int(XAMONXX_MODE_ID, 0, XAMONXX_NUM_MODES - 1);
   apply_mode(&xamonxx_modes[id]);
   cfg.debug = XAMONXX_DEBUG;

#if XAMONXX_DEBUG
   /* Debug zip: cool60 is the safe default until mode.txt says otherwise. */
   apply_mode(&xamonxx_modes[1]);
   find_log_dir();
   load_mode_file();
#endif

   /* Env overrides (Winlator-style launchers). */
   const char *e = getenv("XAMONXX_MODE");
   if (e && *e) {
      const struct xamonxx_mode *m = find_mode(e);
      if (m) {
         apply_mode(m);
         snprintf(mode_source, sizeof(mode_source), "env");
      }
   }
   e = getenv("XAMONXX_GPU_FLOOR");
   if (e && *e) {
      cfg.gpu_floor = !strcmp(e, "max") ? 1 : clamp_int(atoi(e), 0, 100);
      snprintf(mode_source, sizeof(mode_source), "env");
   }
   e = getenv("XAMONXX_FPS_LIMIT");
   if (e && *e) {
      cfg.fps_limit = clamp_int(atoi(e), 0, 240);
      if (cfg.fps_limit) {
         cfg.target_fps = cfg.fps_limit;
         if (cfg.fps_min > cfg.fps_limit)
            cfg.fps_min = cfg.fps_limit;
      }
      snprintf(mode_source, sizeof(mode_source), "env");
   }
   e = getenv("XAMONXX_ADPF");
   if (e && *e) {
      cfg.adpf = clamp_int(atoi(e), 0, 2);
      snprintf(mode_source, sizeof(mode_source), "env");
   }

   if (cfg.target_fps <= 0)
      cfg.target_fps = cfg.fps_limit ? cfg.fps_limit : 60;
   atomic_store(&cur_limit, cfg.fps_limit);
   mesa_logi(XLOG_PREFIX "mode %s (%s): gpu floor %d, fps limit %d (min %d, guard %d), "
             "target %d, adpf %d, debug %d", cfg.mode, mode_source, cfg.gpu_floor,
             cfg.fps_limit, cfg.fps_min, cfg.guard, cfg.target_fps, cfg.adpf, cfg.debug);
}

const struct xamonxx_config *
xamonxx_get_config(void)
{
   pthread_once(&cfg_once, init_config);
   return &cfg;
}

/* ---- GPU floor status (reported by tu_knl_kgsl.cc) ------------------------- */

static atomic_int floor_level_reported = -1;
static atomic_int floor_ok = -1;
static atomic_int floor_err = 0;

void
xamonxx_report_gpu_floor(int level, int ok, int err)
{
   atomic_store(&floor_level_reported, level);
   atomic_store(&floor_err, err);
   atomic_store(&floor_ok, ok);
}

/* ---- ADPF (Android Performance Hint) --------------------------------------- */

typedef struct APerformanceHintManager APerformanceHintManager;
typedef struct APerformanceHintSession APerformanceHintSession;
typedef APerformanceHintManager *(*pfn_get_manager)(void);
typedef APerformanceHintSession *(*pfn_create_session)(APerformanceHintManager *,
                                                       const int32_t *, size_t, int64_t);
typedef int (*pfn_report_work)(APerformanceHintSession *, int64_t);
typedef int (*pfn_prefer_power)(APerformanceHintSession *, bool);
typedef int (*pfn_update_target)(APerformanceHintSession *, int64_t);

static pfn_report_work adpf_report;
static pfn_update_target adpf_update_target;
static _Atomic(APerformanceHintSession *) adpf_session;
static atomic_bool adpf_started;
/* 0 = not started, 1 = ok, 2 = ok + power-efficient, -1 = unavailable */
static atomic_int adpf_state;

struct adpf_args {
   int32_t tid;
   int64_t target_ns;
   int mode;
};

/* Runs on its own detached thread so a slow binder call can never stall a frame. */
static void *
adpf_thread(void *data)
{
   struct adpf_args a = *(struct adpf_args *)data;
   free(data);

   void *lib = dlopen("libandroid.so", RTLD_NOW | RTLD_LOCAL);
   pfn_get_manager get_manager = lib ? (pfn_get_manager)dlsym(lib, "APerformanceHint_getManager") : NULL;
   pfn_create_session create = lib ? (pfn_create_session)dlsym(lib, "APerformanceHint_createSession") : NULL;
   pfn_report_work report = lib ? (pfn_report_work)dlsym(lib, "APerformanceHint_reportActualWorkDuration") : NULL;
   pfn_prefer_power prefer_power = lib ? (pfn_prefer_power)dlsym(lib, "APerformanceHint_setPreferPowerEfficiency") : NULL;
   pfn_update_target update_target = lib ? (pfn_update_target)dlsym(lib, "APerformanceHint_updateTargetWorkDuration") : NULL;

   APerformanceHintManager *mgr = get_manager ? get_manager() : NULL;
   APerformanceHintSession *s = (mgr && create && report) ? create(mgr, &a.tid, 1, a.target_ns) : NULL;
   if (!s) {
      atomic_store(&adpf_state, -1);
      mesa_logw(XLOG_PREFIX "ADPF unavailable (lib %p, manager %p)", lib, (void *)mgr);
      return NULL;
   }

   int state = 1;
   if (a.mode == 2 && prefer_power && prefer_power(s, true) == 0)
      state = 2;

   adpf_report = report;
   adpf_update_target = update_target;
   atomic_store(&adpf_session, s);
   atomic_store(&adpf_state, state);
   mesa_logi(XLOG_PREFIX "ADPF session ok (tid %d, target %lld ns%s)", (int)a.tid,
             (long long)a.target_ns, state == 2 ? ", power-efficient" : "");
   return NULL;
}

static void
adpf_start_once(const struct xamonxx_config *c)
{
   bool expected = false;
   if (!atomic_compare_exchange_strong(&adpf_started, &expected, true))
      return;

   struct adpf_args *a = malloc(sizeof(*a));
   if (!a)
      return;
   a->tid = (int32_t)gettid();
   a->target_ns = 1000000000ll / c->target_fps;
   a->mode = c->adpf;

   pthread_t t;
   pthread_attr_t attr;
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
   if (pthread_create(&t, &attr, adpf_thread, a) != 0) {
      free(a);
      atomic_store(&adpf_state, -1);
   }
   pthread_attr_destroy(&attr);
}

/* ---- thermal sensors (read-only, found by name) ---------------------------- */

#define MAX_ZONES 48

static int zone_skin = -1, zone_batt = -1;
static int zones_gpu[MAX_ZONES], n_gpu;
static int zones_cpu[MAX_ZONES], n_cpu;

static bool
read_small(const char *path, char *buf, size_t size)
{
   int fd = open(path, O_RDONLY | O_CLOEXEC);
   if (fd < 0)
      return false;
   ssize_t n = read(fd, buf, size - 1);
   close(fd);
   if (n <= 0)
      return false;
   buf[n] = '\0';
   return true;
}

static bool
read_long(const char *path, long *out)
{
   char buf[64];
   if (!read_small(path, buf, sizeof(buf)))
      return false;
   char *end;
   errno = 0;
   long v = strtol(buf, &end, 10);
   if (errno || end == buf)
      return false;
   *out = v;
   return true;
}

static void
discover_zones_impl(void)
{
   int skin_shell = -1, skin_sys = -1;
   for (int i = 0; i < 200; i++) {
      char path[96], type[64];
      snprintf(path, sizeof(path), "/sys/class/thermal/thermal_zone%d/type", i);
      if (!read_small(path, type, sizeof(type)))
         continue;
      type[strcspn(type, "\r\n")] = '\0';
      if (!strcmp(type, "tz_shell"))
         skin_shell = i;
      else if (!strcmp(type, "sys-therm-0"))
         skin_sys = i;
      else if (!strcmp(type, "battery"))
         zone_batt = i;
      else if (!strncmp(type, "gpuss-", 6) && n_gpu < MAX_ZONES)
         zones_gpu[n_gpu++] = i;
      else if ((!strncmp(type, "cpu-0-", 6) || !strncmp(type, "cpu-1-", 6)) && n_cpu < MAX_ZONES)
         zones_cpu[n_cpu++] = i;
   }
   zone_skin = skin_shell >= 0 ? skin_shell : skin_sys;
}

/* Temperature in 0.1 °C, or INT32_MIN if unreadable. */
static int
zone_temp_dc(int zone)
{
   if (zone < 0)
      return INT32_MIN;
   char path[96];
   long v;
   snprintf(path, sizeof(path), "/sys/class/thermal/thermal_zone%d/temp", zone);
   if (!read_long(path, &v))
      return INT32_MIN;
   return (int)(v / 100); /* millidegree -> decidegree */
}

static int
zones_max_dc(const int *zones, int n)
{
   int best = INT32_MIN;
   for (int i = 0; i < n; i++) {
      int t = zone_temp_dc(zones[i]);
      if (t > best)
         best = t;
   }
   return best;
}

static pthread_once_t zones_once = PTHREAD_ONCE_INIT;

static void
discover_zones(void)
{
   pthread_once(&zones_once, discover_zones_impl);
}

/* ---- frame statistics + limiter ------------------------------------------- */

static pthread_mutex_t present_lock = PTHREAD_MUTEX_INITIALIZER;
static uint64_t next_deadline;  /* limiter cadence */
static uint64_t last_present;   /* timestamp of previous paced present */

struct frame_stats {
   uint64_t frames;
   uint64_t sum_ns;
   uint64_t max_ns;
   uint64_t slow;      /* frames slower than 1.5x target */
   uint64_t slept_ns;
};
static struct frame_stats stats;  /* protected by present_lock */

#if XAMONXX_DEBUG
static void start_logger_once(void);
#endif

/* ---- thermal guard (Monster modes) ----------------------------------------- */

/* Thresholds apply to 3-sample (6 s) averages. GPU is the heat source; single CPU
 * hot-spot sensors spike by 5-10 °C for a second, so the CPU threshold is higher. */
#define GUARD_GPU_HOT_DC   920  /* 92.0 C average GPU: step the limit down */
#define GUARD_CPU_HOT_DC   1000 /* 100.0 C average CPU: step the limit down */
#define GUARD_GPU_COOL_DC  860  /* both at/below these for 20 s: step back up */
#define GUARD_CPU_COOL_DC  920
#define GUARD_AVG_N        3
#define GUARD_COOL_SAMPLES 10
#define GUARD_STEP_FPS     15

static atomic_bool guard_started;

static void
guard_set_limit(int limit, int temp_dc)
{
   int old = atomic_exchange(&cur_limit, limit);
   atomic_fetch_add(&guard_steps, 1);
   mesa_logi(XLOG_PREFIX "thermal guard: %d.%d C -> fps limit %d -> %d", temp_dc / 10,
             temp_dc % 10, old, limit);
   APerformanceHintSession *s = atomic_load(&adpf_session);
   if (s && adpf_update_target)
      adpf_update_target(s, 1000000000ll / limit);
}

/* Every 2 s, on 6 s averages: GPU >= 92 C or CPU >= 100 C -> limit -15 fps (not
 * below fps_min), then wait 3 fresh samples; GPU <= 86 C and CPU <= 92 C for 20 s
 * -> limit +15 fps (not above fps_limit). Simulated on the owner's V0.3/V0.4 logs. */
static void *
guard_thread(void *unused)
{
   (void)unused;
   discover_zones();
   int gpu_hist[GUARD_AVG_N], cpu_hist[GUARD_AVG_N];
   int n_hist = 0, wait = 0, cool = 0;
   for (;;) {
      struct timespec two_s = { .tv_sec = 2, .tv_nsec = 0 };
      while (nanosleep(&two_s, &two_s) == -1 && errno == EINTR)
         ;
      int g = zones_max_dc(zones_gpu, n_gpu);
      int p = zones_max_dc(zones_cpu, n_cpu);
      if (g == INT32_MIN || p == INT32_MIN)
         continue;
      memmove(gpu_hist + 1, gpu_hist, sizeof(int) * (GUARD_AVG_N - 1));
      memmove(cpu_hist + 1, cpu_hist, sizeof(int) * (GUARD_AVG_N - 1));
      gpu_hist[0] = g;
      cpu_hist[0] = p;
      if (n_hist < GUARD_AVG_N) {
         n_hist++;
         continue;
      }
      int ga = 0, ca = 0;
      for (int i = 0; i < GUARD_AVG_N; i++) {
         ga += gpu_hist[i];
         ca += cpu_hist[i];
      }
      ga /= GUARD_AVG_N;
      ca /= GUARD_AVG_N;
      if (wait) {
         wait--;
         continue;
      }

      int lim = atomic_load(&cur_limit);
      if (ga >= GUARD_GPU_HOT_DC || ca >= GUARD_CPU_HOT_DC) {
         cool = 0;
         if (lim > cfg.fps_min) {
            int n = lim - GUARD_STEP_FPS;
            guard_set_limit(n < cfg.fps_min ? cfg.fps_min : n, ga > ca ? ga : ca);
            wait = GUARD_AVG_N;
         }
      } else if (ga <= GUARD_GPU_COOL_DC && ca <= GUARD_CPU_COOL_DC) {
         if (++cool >= GUARD_COOL_SAMPLES && lim < cfg.fps_limit) {
            int n = lim + GUARD_STEP_FPS;
            guard_set_limit(n > cfg.fps_limit ? cfg.fps_limit : n, ga > ca ? ga : ca);
            cool = 0;
         }
      } else {
         cool = 0;
      }
   }
   return NULL;
}

static void
guard_start_once(void)
{
   bool expected = false;
   if (!atomic_compare_exchange_strong(&guard_started, &expected, true))
      return;
   pthread_t t;
   pthread_attr_t attr;
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
   if (pthread_create(&t, &attr, guard_thread, NULL) != 0)
      mesa_logw(XLOG_PREFIX "cannot start thermal guard");
   pthread_attr_destroy(&attr);
}

void
xamonxx_on_present(void)
{
   const struct xamonxx_config *c = xamonxx_get_config();

   if (c->adpf)
      adpf_start_once(c);
   if (c->guard && c->fps_limit)
      guard_start_once();
#if XAMONXX_DEBUG
   start_logger_once();
#endif

   const uint64_t target = 1000000000ull / (uint64_t)c->target_fps;
   const int limit = atomic_load(&cur_limit);
   const uint64_t period = limit > 0 ? 1000000000ull / (uint64_t)limit : 0;
   uint64_t slept = 0;

   pthread_mutex_lock(&present_lock);
   uint64_t now = now_ns();
   if (period) {
      if (next_deadline == 0) {
         next_deadline = now + period;
      } else {
         if (now < next_deadline) {
            struct timespec ts = {
               .tv_sec = (time_t)(next_deadline / 1000000000ull),
               .tv_nsec = (long)(next_deadline % 1000000000ull),
            };
            while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, NULL) == EINTR)
               ;
            uint64_t woke = now_ns();
            slept = woke - now;
            now = woke;
         }
         /* Keep the cadence; if we are more than one frame late, restart it
          * from now instead of bursting frames to catch up. */
         if (now > next_deadline + period)
            next_deadline = now + period;
         else
            next_deadline += period;
      }
   }

   uint64_t interval = last_present ? now - last_present : 0;
   last_present = now;
   if (interval) {
      stats.frames++;
      stats.sum_ns += interval;
      if (interval > stats.max_ns)
         stats.max_ns = interval;
      if (interval * 2 > target * 3)
         stats.slow++;
      stats.slept_ns += slept;
   }
   pthread_mutex_unlock(&present_lock);

   APerformanceHintSession *s = atomic_load(&adpf_session);
   if (s && interval > slept) {
      int64_t work = (int64_t)(interval - slept);
      if (work > 0 && work < 1000000000ll)
         adpf_report(s, work);
   }
}

/* ---- evaluation log (debug zip) -------------------------------------------- */

#if XAMONXX_DEBUG

#define LOG_MAX_BYTES (5l * 1024 * 1024)
#define LOG_KEEP 20

static atomic_bool logger_started;

static void
fmt_dc(char *out, size_t size, int dc)
{
   if (dc == INT32_MIN)
      snprintf(out, size, "NA");
   else
      snprintf(out, size, "%d.%d", dc / 10, abs(dc % 10));
}

static void
fmt_long(char *out, size_t size, bool ok, long v)
{
   if (ok)
      snprintf(out, size, "%ld", v);
   else
      snprintf(out, size, "NA");
}

static bool
gpu_busy_pct(long *out)
{
   char buf[64];
   if (read_small("/sys/class/kgsl/kgsl-3d0/gpu_busy_percentage", buf, sizeof(buf))) {
      *out = strtol(buf, NULL, 10);
      return true;
   }
   if (read_small("/sys/class/kgsl/kgsl-3d0/gpubusy", buf, sizeof(buf))) {
      long busy = 0, total = 0;
      if (sscanf(buf, "%ld %ld", &busy, &total) == 2 && total > 0) {
         *out = busy * 100 / total;
         return true;
      }
   }
   if (read_small("/sys/kernel/gpu/gpu_busy", buf, sizeof(buf))) {
      *out = strtol(buf, NULL, 10);
      return true;
   }
   return false;
}

/* Value in Hz or MHz (auto-detected) -> MHz. */
static bool
read_mhz(const char *path, long *out)
{
   long v;
   if (!read_long(path, &v) || v <= 0)
      return false;
   *out = v > 100000 ? v / 1000000 : v;
   return true;
}

static bool
gpu_freq_mhz(long *out)
{
   return read_mhz("/sys/class/kgsl/kgsl-3d0/gpuclk", out) ||
          read_mhz("/sys/class/kgsl/kgsl-3d0/clock_mhz", out) ||
          read_mhz("/sys/kernel/gpu/gpu_clock", out) ||
          read_mhz("/sys/class/kgsl/kgsl-3d0/devfreq/cur_freq", out);
}

/* Highest GPU clock currently allowed (drops when thermal mitigation caps the GPU). */
static bool
gpu_max_mhz(long *out)
{
   return read_mhz("/sys/class/kgsl/kgsl-3d0/max_gpuclk", out) ||
          read_mhz("/sys/kernel/gpu/gpu_max_clock", out) ||
          read_mhz("/sys/class/kgsl/kgsl-3d0/devfreq/max_freq", out);
}

static bool
gpu_thermal_level(long *out)
{
   return read_long("/sys/class/kgsl/kgsl-3d0/thermal_pwrlevel", out);
}

static bool
cpu_freq_mhz(int policy, long *out)
{
   char path[96];
   long khz;
   snprintf(path, sizeof(path), "/sys/devices/system/cpu/cpufreq/policy%d/scaling_cur_freq", policy);
   if (!read_long(path, &khz))
      return false;
   *out = khz / 1000;
   return true;
}

static int
cmp_str(const void *a, const void *b)
{
   return strcmp(*(const char *const *)a, *(const char *const *)b);
}

/* Keep only the newest LOG_KEEP-1 logs so the new one makes LOG_KEEP. */
static void
rotate_logs(void)
{
   DIR *d = opendir(log_dir);
   if (!d)
      return;
   char *names[256];
   int n = 0;
   struct dirent *e;
   while ((e = readdir(d)) && n < 256) {
      size_t len = strlen(e->d_name);
      if (!strncmp(e->d_name, "log_", 4) && len > 4 && !strcmp(e->d_name + len - 4, ".csv"))
         names[n++] = strdup(e->d_name);
   }
   closedir(d);
   qsort(names, n, sizeof(names[0]), cmp_str);
   for (int i = 0; i < n; i++) {
      if (names[i] && i < n - (LOG_KEEP - 1)) {
         char path[800];
         snprintf(path, sizeof(path), "%s/%s", log_dir, names[i]);
         unlink(path);
      }
      free(names[i]);
   }
}

static const char *
floor_status(void)
{
   int ok = atomic_load(&floor_ok);
   return ok < 0 ? "not-requested" : (ok ? "accepted" : "rejected");
}

static const char *
adpf_status(void)
{
   switch (atomic_load(&adpf_state)) {
   case 1: return "ok";
   case 2: return "ok-power-efficient";
   case -1: return "unavailable";
   default: return cfg.adpf ? "pending" : "off";
   }
}

static FILE *
open_log(void)
{
   if (!log_dir[0])
      return NULL;
   rotate_logs();

   time_t t = time(NULL);
   struct tm tm;
   localtime_r(&t, &tm);
   char path[800];
   snprintf(path, sizeof(path), "%s/log_%04d%02d%02d_%02d%02d%02d_%s_%d.csv", log_dir,
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
            cfg.mode, (int)getpid());
   FILE *f = fopen(path, "w");
   if (!f) {
      mesa_logw(XLOG_PREFIX "cannot create log %s: %s", path, strerror(errno));
      return NULL;
   }
   mesa_logi(XLOG_PREFIX "evaluation log: %s", path);

   char proc[256] = "?";
   if (read_small("/proc/self/cmdline", proc, sizeof(proc))) {
      for (size_t i = 0; i + 1 < sizeof(proc) && proc[i + 1]; i++)
         if (proc[i] == '\0')
            proc[i] = ' ';
   }
   fprintf(f, "# xamonxx naga8gen5 %s debug log\n", XAMONXX_VERSION);
   fprintf(f, "# mode=%s source=%s gpu_floor=%d fps_limit=%d target_fps=%d fps_min=%d guard=%d adpf=%d\n",
           cfg.mode, mode_source, cfg.gpu_floor, cfg.fps_limit, cfg.target_fps, cfg.fps_min,
           cfg.guard, cfg.adpf);
   fprintf(f, "# process=%s\n", proc);
   fprintf(f, "# gpu_floor_kgsl: level=%d status=%s errno=%d\n",
           atomic_load(&floor_level_reported), floor_status(), atomic_load(&floor_err));
   fprintf(f, "# zones: skin=%d battery=%d gpu=%d cpu=%d\n", zone_skin, zone_batt, n_gpu, n_cpu);
   fprintf(f, "# slow_frames = frames longer than 1.5x the target frame time\n");
   fprintf(f, "t_s,fps,frametime_avg_ms,frametime_max_ms,slow_frames,limiter_sleep_ms,"
              "gpu_busy_pct,gpu_freq_mhz,cpu0_mhz,cpu6_mhz,temp_skin_c,temp_gpu_max_c,"
              "temp_cpu_max_c,temp_batt_c,gpu_floor_status,adpf_status,gpu_max_mhz,"
              "gpu_thermal_level,fps_limit_now,guard_steps\n");
   fflush(f);
   return f;
}

static void *
logger_thread(void *unused)
{
   (void)unused;
   discover_zones();
   FILE *f = open_log();
   const uint64_t t0 = now_ns();
   uint64_t prev = t0;
   unsigned row = 0;

   for (;;) {
      struct timespec one_s = { .tv_sec = 1, .tv_nsec = 0 };
      while (nanosleep(&one_s, &one_s) == -1 && errno == EINTR)
         ;

      struct frame_stats s;
      pthread_mutex_lock(&present_lock);
      s = stats;
      memset(&stats, 0, sizeof(stats));
      pthread_mutex_unlock(&present_lock);

      uint64_t now = now_ns();
      double elapsed = (double)(now - prev) / 1e9;
      prev = now;
      double fps = elapsed > 0 ? (double)s.frames / elapsed : 0.0;
      double avg_ms = s.frames ? (double)s.sum_ns / (double)s.frames / 1e6 : 0.0;
      double max_ms = (double)s.max_ns / 1e6;

      long busy = 0, gfreq = 0, gmax = 0, glvl = 0, c0 = 0, c6 = 0;
      bool busy_ok = gpu_busy_pct(&busy);
      bool gfreq_ok = gpu_freq_mhz(&gfreq);
      bool gmax_ok = gpu_max_mhz(&gmax);
      bool glvl_ok = gpu_thermal_level(&glvl);
      bool c0_ok = cpu_freq_mhz(0, &c0);
      bool c6_ok = cpu_freq_mhz(6, &c6);
      char sb[16], sg[16], sgm[16], sgl[16], s0[16], s6[16], tskin[16], tgpu[16], tcpu[16], tbat[16];
      fmt_long(sb, sizeof(sb), busy_ok, busy);
      fmt_long(sg, sizeof(sg), gfreq_ok, gfreq);
      fmt_long(sgm, sizeof(sgm), gmax_ok, gmax);
      fmt_long(sgl, sizeof(sgl), glvl_ok, glvl);
      fmt_long(s0, sizeof(s0), c0_ok, c0);
      fmt_long(s6, sizeof(s6), c6_ok, c6);
      fmt_dc(tskin, sizeof(tskin), zone_temp_dc(zone_skin));
      fmt_dc(tgpu, sizeof(tgpu), zones_max_dc(zones_gpu, n_gpu));
      fmt_dc(tcpu, sizeof(tcpu), zones_max_dc(zones_cpu, n_cpu));
      fmt_dc(tbat, sizeof(tbat), zone_temp_dc(zone_batt));

      double t_s = (double)(now - t0) / 1e9;
      if (f) {
         fprintf(f, "%.1f,%.1f,%.2f,%.2f,%llu,%.1f,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d,%d\n",
                 t_s, fps, avg_ms, max_ms, (unsigned long long)s.slow, (double)s.slept_ns / 1e6,
                 sb, sg, s0, s6, tskin, tgpu, tcpu, tbat, floor_status(), adpf_status(), sgm, sgl,
                 atomic_load(&cur_limit), atomic_load(&guard_steps));
         fflush(f);
         if (ftell(f) > LOG_MAX_BYTES) {
            fprintf(f, "# log size limit reached, logging stopped\n");
            fclose(f);
            f = NULL;
         }
      }

      if (++row % 10 == 0) {
         mesa_logi(XLOG_PREFIX "t=%.0fs fps=%.1f max=%.1fms gpu=%s%% %sMHz skin=%s gpu=%s cpu=%s",
                   t_s, fps, max_ms, sb, sg, tskin, tgpu, tcpu);
      }
   }
   return NULL;
}

static void
start_logger_once(void)
{
   bool expected = false;
   if (!atomic_compare_exchange_strong(&logger_started, &expected, true))
      return;
   pthread_t t;
   pthread_attr_t attr;
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
   if (pthread_create(&t, &attr, logger_thread, NULL) != 0)
      mesa_logw(XLOG_PREFIX "cannot start logger thread");
   pthread_attr_destroy(&attr);
}

#endif /* XAMONXX_DEBUG */
