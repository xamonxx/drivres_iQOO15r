---
name: known-issues
description: Masalah terbuka & yang sudah selesai
updated: 2026-11-23
type: issue
---

## Terbuka
- **Layar/app di 60 Hz** — benchmark V0.1 melaporkan `@ 60Hz`; >60 fps tidak mungkin terlihat sampai refresh HP
  (Settings → Display → refresh rate tinggi / Game Space per-game) & Target FPS app dinaikkan.
- **Stutter (min fps 8.8–28)** di benchmark V0.1 — target V0.2 GPU floor; perlu data uji.
- **Dukungan KGSL PERC constraint UNKNOWN** — jika varian balanced ditolak kernel, floor tidak aktif (turbo pakai PWR_MAX, lebih kompatibel).
- **Storage HP penuh (88%, ±27 GB bebas)** — GTA V butuh ±95–110 GB. Owner harus kosongkan storage dulu.
- **PC owner tanpa virtualisasi** — build lokal (WSL2/Docker) tidak bisa; andalkan CI.
- **Turnip a8xx masih matang bertahap** — kemungkinan glitch/crash di game tertentu; fallback V39 / Rutabaga.

## Selesai
- 2026-11-23 — Cek simbol salah (`vk_icd*`) menggagalkan CI → diganti cek `HMI`.
