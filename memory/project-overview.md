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
- **V0.1 RILIS**: CI run 45175125137 sukses (flavor perf: O3 + ThinLTO + oryon-1) → GitHub Release `v0.1`.
- Zip lolos `tools/audit_driver.py` (PASS). Salinan di `dist/` dan `C:\Users\Administrator\Downloads\GTA5_Drivers\`.
- CI run #1 (45173761924) gagal hanya karena cek simbol salah (`vk_icd*`) → diganti cek `HMI`.
- Driver: **NOT TESTED** di HP — menunggu laporan owner.

## Next steps
1. Owner uji sesuai `docs/TESTING.md` (V0.1 vs V39) → catat di `test-results.md`.
2. Berdasarkan hasil: V0.2 (mis. tambah patch AAA dari V39-Patched, atau mundur ke flavor safe bila crash).

## Lingkungan
- PC owner Windows 11; virtualisasi BIOS mati → WSL2/Docker tidak jalan → build lewat GitHub Actions.
- gh CLI login sebagai `xamonxx` (aktif) dan `Trlgy-17` (tidak punya akses push ke repo ini).
- Driver pembanding di PC: `C:\Users\Administrator\Downloads\GTA5_Drivers\` (V39, V39 Patched, Rutabaga v3, Qualcomm 891.14, Banners A8xx).
