# BUILD — xamonxx naga8gen5

## Pin versi saat ini (V0.2)
| Komponen | Nilai |
|---|---|
| Mesa | `c5ef4bdfabf2e1a76d5e41ab8e5d75ddbc121275` (gitlab.freedesktop.org/mesa/mesa) |
| Patch upstream | StevenMXZ/Adreno-Tools-Drivers branch `A8xx` @ `4b796491db77b896852354e5484a0ac839e63adc` |
| Toolchain | Android NDK r29 (clang 21, ld.lld), target `aarch64-linux-android35/36` |
| Build system | meson + ninja, cross-file Android aarch64 |
| KMD | KGSL (`-Dfreedreno-kmds=kgsl`) |

Kombinasi Mesa+patch di atas = yang lolos CI StevenMXZ pada 2026-11-23 (patch pasti apply).

## Alur script `build_naga8gen5.sh`
1. `check_deps` — git meson ninja unzip curl flex bison zip glslangValidator python3
2. `prepare_workdir` — unduh NDK r29, fetch Mesa commit yang di-pin (shallow)
3. `apply_unified_patches` — patch A8xx (device ID 829 `0x44030A00`, mesh/compute emulation, LRZ, UBWC, gralloc AIMapper, KGSL recovery, heap usage) + verifikasi `grep` bahwa entri Adreno 829 ada
4. `apply_android_ndk_fixes` — sed fix kompatibilitas NDK r29
5. `build_android` — meson setup + ninja install
6. `package_driver` — tulis `meta.json`, zip `-9`, cek `NEEDED` & ekspor simbol ICD

## Flavor
| FLAVOR | CFLAGS tambahan | LDFLAGS tambahan | NDEBUG |
|---|---|---|---|
| `perf` (default) | `-O3 -fno-plt -flto=thin -mcpu=oryon-1` (fallback `-march=armv8.6-a+i8mm+bf16 -mtune=cortex-x4`) | `-flto=thin -Wl,-O2 -Wl,--gc-sections -Wl,--icf=safe` | ya |
| `safe` | — (flag release upstream) | — | tidak |

CI menjalankan `perf`; bila gagal otomatis `safe` (zip bersuffix `-safe`).

Catatan: `-mcpu=oryon-1` = tuning Oryon generasi 1 (subset fitur Oryon V3, aman). SVE2/SME sengaja **tidak** diaktifkan
paksa — gain untuk kode driver tidak terbukti, risiko crash ada.

## Varian GPU floor (V0.2+, patch `patches/xamonxx/0001_kgsl_gpu_floor.py`)
| VARIANT | `XAMONXX_GPU_FLOOR` | Efek | Zip |
|---|---|---|---|
| `base` | 0 | tanpa floor (= perilaku V0.1) | `xamonxx_naga8gen5V0.2-base.zip` (tidak dirilis default) |
| `balanced` | 60 | clock GPU tidak turun di bawah ~60% max saat game submit | `xamonxx_naga8gen5V0.2-balanced.zip` |
| `turbo` | 1 (PWR_MAX) | clock GPU di level maksimum yang diizinkan termal | `xamonxx_naga8gen5V0.2-turbo.zip` |

Mekanisme: context KGSL dibuat dengan `KGSL_CONTEXT_PWR_CONSTRAINT`, lalu `IOCTL_KGSL_SETPROPERTY`
`KGSL_PROP_PWR_CONSTRAINT` (`KGSL_CONSTRAINT_PWRLEVEL`). Kernel menerapkan per command batch dan
membiarkannya kedaluwarsa saat game berhenti submit. Throttling termal tetap berlaku.
Override runtime (Winlator env var / Mesa option): `XAMONXX_GPU_FLOOR=0..100|max`.
Bila kernel menolak: log `xamonxx naga8gen5: KGSL rejected GPU floor ...` dan driver jalan tanpa floor.

## Cara build
- **CI (disarankan):** GitHub → Actions → *Build xamonxx naga8gen5 driver* → Run workflow. Hasil: Release `v<VERSION>` + artifact per varian.
- **Lokal (Linux x86_64 / WSL2):** `VARIANT=balanced bash build_naga8gen5.sh` (VARIANT=base|balanced|turbo, FLAVOR=perf|safe). Output: `turnip_workdir/xamonxx_naga8gen5V<VERSION>-<VARIANT>.zip`.
  PC owner saat ini: virtualisasi BIOS mati → WSL2/Docker tidak bisa → pakai CI.

## Checklist naik versi
1. Ubah `MESA_COMMIT` / patch / flag di script.
2. Ubah `DRIVER_NAME`, `PKG_VERSION`, `ZIP_NAME` di script + tag/judul release & nama artifact di `.github/workflows/build.yml`.
3. Push → CI → unduh ke `dist/` → `python -I tools/audit_driver.py dist/<zip>`.
4. Catat di `memory/build-history.md` dan `memory/decisions.md`.
