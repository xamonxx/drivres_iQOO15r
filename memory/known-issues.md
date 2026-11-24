---
name: known-issues
description: Masalah terbuka & yang sudah selesai
updated: 2026-11-24
type: issue
---

## Terbuka
- **CPU dibatasi OS di 2361.6 / 2476.8 MHz saat main** (log V0.3, semua mode) — bukan driver. Coba Game Space + Monster Mode
  untuk `com.thegwchr.fuzzy`; verifikasi lewat kolom `cpu0_mhz`/`cpu6_mhz`.
- **Monster90 GPU-bound (99% busy, ±57 fps)** — perlu setting/resolusi lebih ringan; resolusi sesi V0.3 tidak tercatat.
- **Monster120 panas (GPU 104°C, CPU 109°C dalam 2 menit)** — V0.4 thermal guard; perlu verifikasi di log V0.4.
- **Clock GPU tidak terbaca (`gpu_freq_mhz` NA)** — V0.4 menambah fallback path; bila tetap NA = dibatasi SELinux.
- **Layar/app di 60 Hz** — untuk >60 fps refresh HP & Target FPS app harus dinaikkan.
- **Storage HP penuh (88%, ±27 GB bebas)** — GTA V butuh ±95–110 GB.
- **PC owner tanpa build lokal** — andalkan CI.
- **Turnip a8xx masih matang bertahap** — fallback V39 / Rutabaga.

## Selesai
- 2026-11-24 — Cool60 V0.3 tersendat (V-Sync miss 25/42 detik) → V0.4: limiter off untuk Cool60, floor 60%.
- 2026-11-24 — ADPF di proses Box64 / izin storage log / KGSL PERC: semua **terbukti jalan** (log V0.3).
- 2026-11-23 — Cek simbol salah (`vk_icd*`) menggagalkan CI → diganti cek `HMI`.
