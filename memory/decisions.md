---
name: decisions
description: Log keputusan teknis (terbaru di atas)
updated: 2026-11-24
type: decision
---

### 2026-11-24 — V0.4: tuning dari log debug V0.3
- **Cool60:** limiter driver OFF (pacing oleh V-Sync + Target FPS 60 game; dua pacer → V-Sync miss), GPU floor 40→60%,
  ADPF session biasa (power-efficient membiarkan CPU turun ke 0.5–0.9 GHz saat main). Field baru `target_fps` dipisah dari `fps_limit`.
- **Monster90/120:** thermal guard di driver — sensor `gpuss-*`/`cpu-*` tiap 2 s; ≥95°C dua kali → limit −15 fps (min 60 / 90),
  ≤85°C 30 s → +15. ADPF target ikut di-update. Simulasi pada log monster120 V0.3: 120→105 (t=39 s) →90 (t=43 s).
  Alasan: GPU 104°C/CPU 109°C dalam 2 menit; mengurangi frame satu-satunya tuas panas yang sah di level driver.
- **Logger:** fallback clock GPU (`gpuclk`, `clock_mhz`, `/sys/kernel/gpu/gpu_clock`, devfreq), kolom baru `gpu_max_mhz`,
  `gpu_thermal_level`, `fps_limit_now`, `guard_steps`. Analyzer membuang fase loading (GPU <30%) & 3 s terakhir.
- **Tidak diubah:** GPU floor Monster tetap PWR_MAX (GPU sudah 99% busy, floor bukan sumber panas tambahan).
- **Tidak bisa di driver:** batas CPU 2.36/2.48 GHz dari OS → owner coba Game Space + Monster Mode.

### 2026-11-24 — V0.3: mode Cool60 / Monster90 / Monster120 + debug zip dengan log
- **Konteks:** owner minta driver per mode: 60 fps stabil & adem; 90/120 fps "monster clock CPU"; grafis tidak buriq;
  plus log otomatis untuk evaluasi. Launcher GTA V **tidak punya env var** → semua nilai dibenamkan per zip.
- **Keputusan:**
  - Modul `patches/xamonxx/src/xamonxx.{c,h}` (dipasang oleh `0002_xamonxx_runtime.py`): tabel mode, FPS limiter,
    ADPF hint, logger debug. Hook present di `vk_common_QueueSignalReleaseImageANDROID` + `wsi_common_queue_present`.
  - 0001 diubah: level floor dari `xamonxx_get_config()` (macro `XAMONXX_GPU_FLOOR` dihapus), hasil ioctl dilaporkan.
  - Cool60 = floor 40% + limit 60 + ADPF power-efficient. Monster = floor PWR_MAX + limit 90/120 + ADPF boost.
  - Debug zip: mode dari `Download/xamonxx/mode.txt` (default cool60), CSV 1 baris/detik, thread terpisah.
  - Plan awal menyebut patch 0002/0003/0004 terpisah; digabung jadi satu modul C + satu skrip karena berbagi state.
  - `-Wl,-z,max-page-size=16384` karena `pagesize.max=16384` di HP.
- **Alasan CPU via ADPF:** satu-satunya jalur resmi tanpa root untuk memengaruhi clock CPU. Hanya hint; OS memutuskan.
- **Ditolak:** tulis sysfs cpufreq/governor (dilarang AGENTS.md §6), paksa SVE2/SME (keputusan 2026-11-23 tetap).
- **Risiko:** ADPF di proses Box64 bisa `unavailable` (aman, fallback). Nilai floor 40% belum terukur.
  Limiter + V-Sync game bisa saling menunggu → di Monster set Target FPS launcher Unlimited & V-Sync OFF.

### 2026-11-23 — V0.2: KGSL GPU frequency floor (patch xamonxx/0001)
- **Konteks:** owner minta target 90/120 fps. Benchmark V0.1 ter-cap 60 (Target FPS 60, layar 60 Hz) dengan
  spike min 8.8–28 fps. Satu-satunya tuas performa nyata di level driver yang belum dipakai Turnip:
  `KGSL_PROP_PWR_CONSTRAINT` (dipakai driver Qualcomm untuk performance hint).
- **Keputusan:** patch `patches/xamonxx/0001_kgsl_gpu_floor.py` → tiap context KGSL diberi
  `KGSL_CONTEXT_PWR_CONSTRAINT` + constraint PWRLEVEL. Varian: `balanced` (floor 60% max clock), `turbo` (max clock).
  Override runtime `XAMONXX_GPU_FLOOR=0..100|max`. Bila kernel menolak → warning, lanjut tanpa floor.
- **Keamanan:** throttling termal KGSL tetap membatasi level maksimum; tidak menyentuh sysfs/root.
- **Trade-off:** floor lebih tinggi → frame time lebih stabil tapi lebih panas & boros (turbo paling panas).
- **Tidak bisa dilakukan di driver:** refresh rate layar & present mode (diatur Android/app), cap FPS game.
- **Belum terverifikasi:** apakah KGSL gen8 di kernel 6.12 menerima PERC (balanced) — cek log `xamonxx naga8gen5: KGSL rejected`.

### 2026-11-23 — Cek entry point pakai simbol `HMI`
- **Konteks:** CI run #1 gagal di cek `vk_icdGetInstanceProcAddr`.
- **Keputusan:** verifikasi `HMI` (Android hw_module) — sama dengan Turnip V39 & Rutabaga.
- **Alasan:** loader Vulkan Android/AdrenoTools memuat driver via `HMI`.

### 2026-11-23 — Build via GitHub Actions
- **Konteks:** WSL2/Docker di PC owner tidak jalan (virtualisasi BIOS mati).
- **Keputusan:** build di CI repo `xamonxx/drivres_iQOO15r`, release otomatis `v0.1`.

### 2026-11-23 — Basis = StevenMXZ A8xx @ 4b79649 + Mesa c5ef4bd (di-pin)
- **Alasan:** kombinasi yang lolos CI upstream hari itu; V39 dari repo ini terbukti mengandung Adreno 829 dan paling populer (66k unduhan).
- **Alternatif ditolak:** Mesa `main` tak di-pin (patch bisa gagal apply), Banners nightly (tidak dites di perangkat).

### 2026-11-23 — Flag perf: -O3, ThinLTO, -mcpu=oryon-1, NDEBUG, lld gc-sections/icf
- **Alasan:** meniru build "perf" Banners-Turnip yang sudah terbukti + tuning CPU Oryon.
- **Tidak dipakai:** paksa SVE2/SME (risiko crash, gain belum terbukti).
- **Fallback:** flavor `safe` otomatis bila perf gagal build.
- **Ekspektasi:** gain ±0–5% (ESTIMATED, belum diukur).
