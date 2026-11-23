---
name: known-issues
description: Masalah terbuka & yang sudah selesai
updated: 2026-11-23
type: issue
---

## Terbuka
- **Storage HP penuh (88%, ±27 GB bebas)** — GTA V butuh ±95–110 GB. Owner harus kosongkan storage dulu.
- **PC owner tanpa virtualisasi** — build lokal (WSL2/Docker) tidak bisa; andalkan CI.
- **Turnip a8xx masih matang bertahap** — kemungkinan glitch/crash di game tertentu; fallback V39 / Rutabaga.

## Selesai
- 2026-11-23 — Cek simbol salah (`vk_icd*`) menggagalkan CI → diganti cek `HMI`.
