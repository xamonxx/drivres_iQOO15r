---
name: build-history
description: Riwayat setiap build (terbaru di atas)
updated: 2026-11-24
type: build
---

| Tanggal | Versi | Run CI | Mesa | Patch upstream | Flavor | Hasil | SHA256 zip |
|---|---|---|---|---|---|---|---|
| 2026-11-24 | V0.4-debug | 45433961982 | c5ef4bd | StevenMXZ 4b79649 + xamonxx/0001+0002 | perf, mode.txt + log CSV (kolom guard & clock GPU) | **SUKSES** → release v0.4; audit PASS; cek CI: thermal guard ter-link, LOAD 0x4000 | 09e88ab974b58391215af5b9ff93a3639e006d1ca6611b5a31cdd6a989dd5c36 |
| 2026-11-24 | V0.4-monster120 | 45433961982 | c5ef4bd | StevenMXZ 4b79649 + xamonxx/0001+0002 | perf, floor max, limit 120 + guard min 90 | **SUKSES** → release v0.4; audit PASS; cek CI: thermal guard ter-link, LOAD 0x4000 | c4f5c115a0b9f17c641706b85d5e2d0c93cc239d87e44effbb47268775edcad6 |
| 2026-11-24 | V0.4-monster90 | 45433961982 | c5ef4bd | StevenMXZ 4b79649 + xamonxx/0001+0002 | perf, floor max, limit 90 + guard min 60 | **SUKSES** → release v0.4; audit PASS; cek CI: thermal guard ter-link, LOAD 0x4000 | bfac505f9a1c3a3dcc93617fa5b5c29f831e1fae07a7529bdbef20e256be8d5b |
| 2026-11-24 | V0.4-cool60 | 45433961982 | c5ef4bd | StevenMXZ 4b79649 + xamonxx/0001+0002 | perf, floor 60%, tanpa limiter, ADPF 16.7 ms | **SUKSES** → release v0.4; audit PASS; cek CI: thermal guard ter-link, LOAD 0x4000 | cfc3f8b954d0e17c5fb4906f4b405bdd116186ba46921453ce40aab9271679e0 |
| 2026-11-24 | V0.3-debug | 45427857301 | c5ef4bd | StevenMXZ 4b79649 + xamonxx/0001+0002 | perf, mode.txt + log CSV | **SUKSES** → release v0.3; audit PASS; NEEDED tidak berubah (tanpa libandroid); LOAD align 0x4000 | 89cb90af4d2772de37d385bac0988672948c7b7fd2c81cee7ed8313d510d4354 |
| 2026-11-24 | V0.3-monster120 | 45427857301 | c5ef4bd | StevenMXZ 4b79649 + xamonxx/0001+0002 | perf, floor max, limit 120, ADPF boost | **SUKSES** → release v0.3; audit PASS; NEEDED tidak berubah (tanpa libandroid); LOAD align 0x4000 | ff06a5f931c8057490ad3ce94ce34e32b385478bc804126497f798af3b9f272f |
| 2026-11-24 | V0.3-monster90 | 45427857301 | c5ef4bd | StevenMXZ 4b79649 + xamonxx/0001+0002 | perf, floor max, limit 90, ADPF boost | **SUKSES** → release v0.3; audit PASS; NEEDED tidak berubah (tanpa libandroid); LOAD align 0x4000 | c072297fd837ab5d20219772288123604b2d918d772faa5ee64163b945811850 |
| 2026-11-24 | V0.3-cool60 | 45427857301 | c5ef4bd | StevenMXZ 4b79649 + xamonxx/0001+0002 | perf, floor 40%, limit 60, ADPF hemat | **SUKSES** → release v0.3; audit PASS; NEEDED tidak berubah (tanpa libandroid); LOAD align 0x4000 | 6c96093a5de6e3ad0c87e311d1086a34d338af85c03544413b841cfdcb53d13d |
| 2026-11-23 | V0.2-turbo | 45196163906 | c5ef4bd | StevenMXZ 4b79649 + xamonxx/0001 | perf, floor=max | **SUKSES** → release v0.2; audit PASS; kode floor ter-link | 3e30e3c86d20f09898e40975c432d6b8c15d1e459865b7d1c73f7c247142ce29 |
| 2026-11-23 | V0.2-balanced | 45196163906 | c5ef4bd | StevenMXZ 4b79649 + xamonxx/0001 | perf, floor=60% | **SUKSES** → release v0.2; audit PASS; kode floor ter-link | b73758eb4d27088025f9146847ba9e39a43523a1c5c3a5471fd09b855cf36e2f |
| 2026-11-23 | V0.1 | 45175125137 | c5ef4bd | StevenMXZ 4b79649 | perf | **SUKSES** → release v0.1; audit PASS (aarch64, Adreno 829, HMI diekspor); .so 10.39 MB (V39 8.37 MB; lebih besar karena inlining O3) | d90665df8acb4a5d338f90e9e7e374e3a75cf1f04696089361ff21636ead73ca |
| 2026-11-23 | V0.1 | 45173761924 | c5ef4bd | StevenMXZ 4b79649 | perf + safe | Compile OK keduanya; job GAGAL di cek simbol (salah cek `vk_icd*`). Tidak ada release. NEEDED: libhardware, liblog, libnativewindow, libsync, libm, libz, libdl, libc | — |
