/*
 * xamonxx naga8gen5 — per-mode runtime tuning for Turnip on Adreno 829 (iQOO 15R).
 * Copied into src/vulkan/runtime/ by patches/xamonxx/0002_xamonxx_runtime.py.
 * SPDX-License-Identifier: MIT
 */
#ifndef XAMONXX_H
#define XAMONXX_H

#ifdef __cplusplus
extern "C" {
#endif

struct xamonxx_config {
   const char *mode;   /* "base", "cool60", "monster90", "monster120" */
   int gpu_floor;      /* 0 = off, 1 = KGSL PWR_MAX, 2..100 = % of max GPU clock */
   int fps_limit;      /* driver FPS limiter: 0 = off, otherwise frames per second */
   int target_fps;     /* frame-time target for ADPF + log statistics */
   int fps_min;        /* lowest limit the thermal guard may step down to */
   int guard;          /* 1 = thermal guard lowers the limit when the SoC runs hot */
   int adpf;           /* 0 = off, 1 = session, 2 = power-efficient session */
   int debug;          /* 1 = evaluation log enabled (debug zip) */
};

/* Resolved once: compiled-in mode, then mode.txt (debug zip), then env vars. */
const struct xamonxx_config *xamonxx_get_config(void);

/* Called by the KGSL backend after requesting the GPU floor on a context. */
void xamonxx_report_gpu_floor(int level, int ok, int err);

/* Called once per present: FPS limiter, ADPF work report, frame statistics. */
void xamonxx_on_present(void);

#ifdef __cplusplus
}
#endif

#endif /* XAMONXX_H */
