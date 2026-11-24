---
name: known-issues
description: Masalah terbuka & yang sudah selesai
updated: 2026-11-24
type: issue
---

## Terbuka
- **CPU dibatasi OS di 2361.6 / 2476.8 MHz saat main** (log V0.3 & V0.4) — **tetap** walau Game Space + Monster Mode ON
  (log V0.4). Di luar kendali driver; GPU (98–99% busy) tetap bottleneck utama di Monster.
- **Monster90 GPU-bound (99% busy, ±57 fps)** — perlu setting/resolusi lebih ringan; resolusi sesi V0.3 tidak tercatat.
- **Monster120 panas (GPU 104°C, CPU 109°C dalam 2 menit, log V0.3)** — guard V0.5 (rata-rata 6 s); verifikasi sesi ≥10 menit.
- **120 fps tidak tercapai** — terbaik ±96 fps (GPU 98% busy) tanpa command line; perlu profil B + resolusi rendah.
- **Clock GPU tidak terbaca** — semua jalur sysfs (gpuclk, clock_mhz, /sys/kernel/gpu, devfreq) NA di log V0.4 → diblok SELinux.
- **Layar/app di 60 Hz** — untuk >60 fps refresh HP & Target FPS app harus dinaikkan.
- **Storage HP penuh (88%, ±27 GB bebas)** — GTA V butuh ±95–110 GB.
- **PC owner tanpa build lokal** — andalkan CI.
- **Turnip a8xx masih matang bertahap** — fallback V39 / Rutabaga.

## Selesai
- 2026-11-24 — Cool60 + Target FPS Unlimited tanpa cap (93 fps, GPU 98°C) → V0.5 cap pengaman 63.
- 2026-11-24 — Guard V0.4 turun ke 60 karena lonjakan CPU sesaat → V0.5 rata-rata 6 s + ambang GPU.
- 2026-11-24 — Cool60 tersendat V0.3 **terkonfirmasi beres** di V0.4 (V-Sync miss 2/98 s).
- 2026-11-24 — Cool60 V0.3 tersendat (V-Sync miss 25/42 detik) → V0.4: limiter off untuk Cool60, floor 60%.
- 2026-11-24 — ADPF di proses Box64 / izin storage log / KGSL PERC: semua **terbukti jalan** (log V0.3).
- 2026-11-23 — Cek simbol salah (`vk_icd*`) menggagalkan CI → diganti cek `HMI`.
