---
name: decisions
description: Log keputusan teknis (terbaru di atas)
updated: 2026-11-23
type: decision
---

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
