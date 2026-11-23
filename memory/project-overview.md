---
name: project-overview
description: Tujuan project, status terkini, dan langkah berikutnya
updated: 2026-11-23
type: project
---

## Tujuan
Driver Vulkan Turnip custom **xamonxx naga8gen5** untuk iQOO 15R (Adreno 829, SD 8 Gen 5) agar GTA V Legacy
(dan game PC lain) jalan sebaik mungkin di GameHub/Winlator. Fokus owner: FPS lancar, suhu adem, grafis menyesuaikan.

## Status (2026-11-23)
- **V0.1** (release `v0.1`): TESTED oleh owner di GTA V — ter-load, stabil, avg 60 fps **pada cap 60 / layar 60 Hz**,
  min 8.8–28 fps (stutter). Detail: `test-results.md`.
- **V0.2** (release `v0.2`, CI 45196163906, audit PASS, salinan di DownloadsGTA5_Drivers): + patch `xamonxx/0001` KGSL GPU frequency floor, varian `balanced` (60%) & `turbo` (max).
  Target owner: 90 fps (base) / 120 fps. NOT TESTED.

## Next steps
1. Owner set layar 120/144 Hz + Target FPS 120/Unlimited + V-Sync OFF, lalu benchmark: V0.1 vs V0.2-balanced vs V0.2-turbo.
2. Dari hasil: pilih floor default; bila 90 belum tercapai → turunkan resolusi (mis. 1280×586) atau setting grafis.

## Lingkungan
- PC owner Windows 11; virtualisasi BIOS mati → WSL2/Docker tidak jalan → build lewat GitHub Actions.
- gh CLI login sebagai `xamonxx` (aktif) dan `Trlgy-17` (tidak punya akses push ke repo ini).
- Driver pembanding di PC: `C:\Users\Administrator\Downloads\GTA5_Drivers\` (V39, V39 Patched, Rutabaga v3, Qualcomm 891.14, Banners A8xx).
