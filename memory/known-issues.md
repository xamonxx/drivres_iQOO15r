---
name: known-issues
description: Masalah terbuka & yang sudah selesai
updated: 2026-11-24
type: issue
---

## Terbuka
- **ADPF di proses game (Box64/Wine) UNKNOWN** — bila `adpf_status=unavailable` di log, mode Monster hanya mengandalkan
  GPU floor + Monster Mode HP.
- **Izin storage launcher untuk log debug UNKNOWN** — bila `Download/xamonxx/` tidak muncul, ambil via folder app/logcat (Shizuku).
- **GPU floor 40% (Cool60) & PERC di KGSL gen8 belum terverifikasi** — lihat `gpu_floor_status` di log debug.
- **Layar/app di 60 Hz** — benchmark V0.1 melaporkan `@ 60Hz`; >60 fps tidak mungkin terlihat sampai refresh HP
  (Settings → Display → refresh rate tinggi / Game Space per-game) & Target FPS app dinaikkan.
- **Stutter (min fps 8.8–28)** di benchmark V0.1 — target V0.2 GPU floor; perlu data uji.
- **Dukungan KGSL PERC constraint UNKNOWN** — jika varian balanced ditolak kernel, floor tidak aktif (turbo pakai PWR_MAX, lebih kompatibel).
- **Storage HP penuh (88%, ±27 GB bebas)** — GTA V butuh ±95–110 GB. Owner harus kosongkan storage dulu.
- **PC owner tanpa virtualisasi** — build lokal (WSL2/Docker) tidak bisa; andalkan CI.
- **Turnip a8xx masih matang bertahap** — kemungkinan glitch/crash di game tertentu; fallback V39 / Rutabaga.

## Selesai
- 2026-11-23 — Cek simbol salah (`vk_icd*`) menggagalkan CI → diganti cek `HMI`.
