---
name: test-results
description: Hasil uji driver di iQOO 15R (terbaru di atas) — format di docs/TESTING.md
updated: 2026-11-24
type: test
---

### 2026-11-24 22:47–22:59 — V0.3-debug — GTA V, 3 sesi (cool60 → monster90 → monster120)
- Sumber: `resault_debug/log_*.csv` dari owner (tidak di-commit). Launcher `com.thegwchr.fuzzy:game`.
- Folder `Download/xamonxx/` + `mode.txt` + CSV **berhasil dibuat** (izin storage OK). GPU floor KGSL **accepted** di semua
  mode (PERC 40% dan PWR_MAX). ADPF **ok** / **ok-power-efficient** → jalan di proses game. `gpu_freq_mhz` selalu NA.
- Sesi pendek (83 / 248 / 127 s) dan beruntun tanpa jeda (skin awal 34.7 → 37.6 → 42.4°C). Resolusi/profil tiap sesi tidak dicatat.
- Loading ±14–39 s pertama: GPU busy 2–5% (bukan gameplay; analyzer kini membuangnya).

| Mode | Gameplay | FPS avg | 1% low (per detik) | ≥95% target | GPU busy | CPU maks (P/Prime) | GPU maks | CPU maks | Skin maks |
|---|---|---|---|---|---|---|---|---|---|
| cool60 | 42 s | 55.8 | 46.9 | 43% (60) | 79% | 2361 / 2476 MHz | 68.1°C | 88.8°C* | 38.0°C |
| monster90 | 231 s | 57.6 | 33.9 | 0% (90) | 99% | 2361 / 2476 MHz | 81.0°C | 96.2°C | 43.1°C |
| monster120 | 110 s | 98.4 | 65.9 | 3% (120) | 98% | 2361 / 2476 MHz | 104.4°C | 109.1°C | 49.4°C |
*cool60 CPU 88.8°C hanya di detik 1 (kompilasi saat load).

**Analisis:**
- **Cool60:** 25 dari 42 detik punya frame 30–40 ms = V-Sync miss (frame tampil 2×) padahal GPU hanya 79% sibuk →
  masalah pacing, bukan tenaga: limiter driver 60 + V-Sync game 60 saling tunggu, dan floor 40% membiarkan GPU di clock rendah.
  CPU Performance sempat turun ke 556–902 MHz saat gameplay (hint power-efficient).
- **Monster90:** GPU 99% sibuk sepanjang sesi → GPU-bound di ±57 fps; limiter 90 tidak pernah aktif. Blok 33–35 fps stabil
  (t=142–146, 181–187, 245–248) = scene berat. Bukan bug driver; butuh setting/resolusi lebih ringan.
- **Monster120:** ±98 fps (setting kemungkinan lebih ringan dari sesi monster90, tidak tercatat) tapi GPU 104°C & CPU 109°C
  dalam 2 menit → tidak layak sesi panjang.
- **CPU dibatasi OS** di 2361.6 / 2476.8 MHz (71% / 65% max hardware) di semua mode; ADPF tidak bisa melewati batas itu.
- Status V0.3: **TESTED (debug)** — stabil tanpa crash; perlu tuning → V0.4.

### 2026-11-23 21:50 — V0.2 (varian UNKNOWN, owner belum menyebut) — GTA V benchmark, cap dilepas
- Command line owner: cityDensity 0.5, lodScale 0.7, grass 1, fogVolumes 0, particle 0, particleShadows 0,
  postFX 1, reflection 1, SSAO 0, shadow 1, texture 1, AF 0. Resolusi 1568×718.
- Benchmark: **avg 77.8 fps**, masih `Display @ 60Hz VSync OFF` (layar 60 Hz → >60 fps tidak tampil, potensi tearing).

| Pass | Avg | Min | Max | ≥60fps | 99% frame < |
|---|---|---|---|---|---|
| 1 | 81.5 | 11.5 | 116.0 | 92% | 22 ms |
| 2 | 74.9 | 54.8 | 95.5 | 99% | 16 ms |
| 3 | 73.8 | 42.4 | 94.9 | 99% | 15 ms |
| 4 | 79.5 | 44.5 | 104.0 | 99% | 15 ms |
| 5 | 79.4 | 3.7 | 133.4 | 90% | 26 ms |

**Analisis:** headroom nyata ±74–81 fps. Pass 2–4 sangat stabil (99% frame ≥60). Spike di pass 1 (load/shader cache)
dan pass 5 (min 3.7, kemungkinan streaming/termal). Untuk 90 fps beban perlu ±15–20% lebih ringan → profil di
`docs/COMMANDLINE.md` + layar 120/144 Hz. Tidak bisa dibandingkan langsung dengan V0.1 (V0.1 diuji dengan cap 60).

### 2026-11-23 21:28 — V0.1 (perf) — GTA V, benchmark bawaan app (5 pass)
- Sumber: screenshot owner (`debugimg/`, tidak di-commit).
- App: launcher GTA V Android (menu Settings / Benchmark Runs, "Import driver (.zip)"), driver terbaca
  `xamonxx naga8gen5 V0.1 Vulkan 1.4.368` → **driver ter-load & stabil, tanpa crash** (VERIFIED).
- Setting: resolusi 1568×718, V-Sync ON, **Target FPS 60**, Pin threads to cores ON, Graphics DEFAULT, Texture NORMAL.
- Benchmark tercatat `Display: 1568x718 (FullScreen) @ 60Hz VSync OFF`.

| Pass | Avg | Min | Max | ≥60fps | ≥30fps | 99% frame < |
|---|---|---|---|---|---|---|
| 1 | 60.0 | 8.8 | 67.3 | 15% | 100% | 18 ms |
| 2 | 59.9 | 28.3 | 67.9 | 11% | 100% | 19 ms |
| 3 | 60.1 | 19.4 | 67.9 | 14% | 99% | 18 ms |
| 4 | 60.1 | 25.0 | 70.9 | 8% | 100% | 17 ms |
| 5 | 59.8 | 19.9 | 82.0 | 16% | 99% | 20 ms |

**Analisis:**
- Rata-rata 60 = **batas cap** (Target FPS 60 + layar 60 Hz), bukan batas driver/GPU → headroom sebenarnya UNKNOWN.
- Spike min 8.8–28 fps & p99 17–20 ms → ada stutter (kemungkinan shader compile di pass 1, ramp clock GPU/CPU).
- Max 67–82 fps → frame individual bisa lebih cepat dari 60.
- **Untuk target 90/120:** wajib uji ulang dengan Target FPS 120/Unlimited, V-Sync OFF, layar HP 120/144 Hz.
- Status V0.1: **TESTED (stabil @60 cap)**.
