---
name: project-overview
description: Tujuan project, status terkini, dan langkah berikutnya
updated: 2026-11-24
type: project
---

## Tujuan
Driver Vulkan Turnip custom **xamonxx naga8gen5** untuk iQOO 15R (Adreno 829, SD 8 Gen 5) agar GTA V Legacy
(dan game PC lain) jalan sebaik mungkin di GameHub/Winlator. Fokus owner: FPS lancar, suhu adem, grafis menyesuaikan.

## Status (2026-11-24)
- **V0.1** (release `v0.1`): TESTED oleh owner di GTA V — ter-load, stabil, avg 60 fps **pada cap 60 / layar 60 Hz**,
  min 8.8–28 fps (stutter). Detail: `test-results.md`.
- **V0.2** (release `v0.2`, CI 45196163906, audit PASS, salinan di `C:\Users\Administrator\Downloads\GTA5_Drivers\`): + patch `xamonxx/0001` KGSL GPU frequency floor, varian `balanced` (60%) & `turbo` (max).
  Target owner: 90 fps (base) / 120 fps. NOT TESTED.
- **V0.3** (2026-11-24): 4 zip — `cool60`, `monster90`, `monster120`, `debug` (mode via `mode.txt` + log CSV).
  Fitur: FPS limiter, ADPF hint, GPU floor per mode, LOAD 16K-aligned, `tools/analyze_log.py`. NOT TESTED.

## Next steps
1. Owner pasang V0.3-debug → cek `Download/xamonxx/` muncul → sesi 15 menit per mode (docs/TESTING.md) → kirim CSV.
2. Agent jalankan `tools/analyze_log.py`, catat ke `test-results.md`; putuskan: floor Cool60 (40%) naik/turun,
   ADPF berguna atau tidak, resolusi profil C bisa naik atau tidak.
3. Bila debug zip crash saat start → curigai ADPF/logger; buat V0.3.1 dengan ADPF off.

## Lingkungan
- PC owner Windows 11; per 2026-11-24 `wsl -l` menunjukkan Ubuntu-24.04 (stopped, tanpa gcc) + docker-desktop (running),
  tapi build tetap lewat GitHub Actions (belum dicoba lokal).
- gh CLI login sebagai `xamonxx` (aktif) dan `Trlgy-17` (tidak punya akses push ke repo ini).
- Driver pembanding di PC: `C:\Users\Administrator\Downloads\GTA5_Drivers\` (V39, V39 Patched, Rutabaga v3, Qualcomm 891.14, Banners A8xx).
