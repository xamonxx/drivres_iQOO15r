---
name: build-history
description: Riwayat setiap build (terbaru di atas)
updated: 2026-11-23
type: build
---

| Tanggal | Versi | Run CI | Mesa | Patch upstream | Flavor | Hasil | SHA256 zip |
|---|---|---|---|---|---|---|---|
| 2026-11-23 | V0.1 | 45175125137 | c5ef4bd | StevenMXZ 4b79649 | perf | **SUKSES** → release v0.1; audit PASS (aarch64, Adreno 829, HMI diekspor); .so 10.39 MB (V39 8.37 MB; lebih besar karena inlining O3) | d90665df8acb4a5d338f90e9e7e374e3a75cf1f04696089361ff21636ead73ca |
| 2026-11-23 | V0.1 | 45173761924 | c5ef4bd | StevenMXZ 4b79649 | perf + safe | Compile OK keduanya; job GAGAL di cek simbol (salah cek `vk_icd*`). Tidak ada release. NEEDED: libhardware, liblog, libnativewindow, libsync, libm, libz, libdl, libc | — |
