---
name: test-results
description: Hasil uji driver di iQOO 15R (terbaru di atas) — format di docs/TESTING.md
updated: 2026-11-23
type: test
---

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
