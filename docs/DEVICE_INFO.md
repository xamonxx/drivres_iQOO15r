# DEVICE INFO — iQOO 15R

> **Referensi permanen untuk AI Agent.** Semua rekomendasi software, model AI, gaming, emulator, development tools, dan optimasi HARUS disesuaikan dengan data perangkat aktual di dokumen ini.
>
> - **Tanggal audit:** 2026-11-23, 11:32–11:40 UTC
> - **Metode:** Audit native on-device (tanpa ADB, tanpa PC, tanpa root, read-only)
> - **Environment audit:** Container Ubuntu 24.04.3 LTS ARM64 bawaan aplikasi OpenChamber (bukan Termux), berjalan langsung di atas Android 17 perangkat
> - **Label data:** `VERIFIED` (terbaca dari perangkat) · `OFFICIAL` (spesifikasi resmi pabrikan/Qualcomm) · `DERIVED` (dihitung dari data aktual) · `ESTIMATED` (estimasi) · `UNKNOWN` (tidak dapat diakses) · `NOT TESTED` (belum diuji)

---

## 1. Executive Summary

**iQOO 15R (vivo I2508)** terkonfirmasi sebagai perangkat aktual — identitas dibaca langsung dari device bridge Android (`manufacturer: vivo`, `brand: iQOO`, `model: I2508`), dan hardware cocok dengan spesifikasi resmi: **Snapdragon 8 Gen 5 (SM8845)**, CPU **Oryon V3 2×3.80 GHz + 6×3.32 GHz**, GPU **Adreno 829** (terbaca dari `/sys/class/kgsl/kgsl-3d0/gpu_model`), RAM **12 GB LPDDR5X Ultra**, storage **256 GB UFS 4.1**.

Perangkat adalah **value-flagship gaming kelas atas (2026)**: sangat kuat untuk native Android gaming, development ARM64 (Python/Node/C), dan local AI hingga kelas 4B–8B parameter (kuantisasi). Batas utama: **storage hampir penuh (88%, sisa ±27 GB)**, **RAM tersedia saat audit hanya ±3.5 GiB** (12 GB fisik dibagi dengan Android), dan **throttling termal pada beban berkelanjutan**. Emulasi PC/Switch kelas berat dan LLM 12B+ tidak direkomendasikan.

Temuan penting environment: agent berjalan di **container Ubuntu terkurung (seccomp, tanpa capabilities)** — bukan root sungguhan, bukan Termux. Beberapa node `/proc` dan `/sys` dibatasi; tidak ada bypass yang dilakukan.

| Domain | Status singkat |
|---|---|
| Native gaming | EXCELLENT (ESTIMATED, kelas flagship) |
| Emulasi retro (PS1/PSP/PS2/GC/Wii) | GOOD–EXCELLENT (NOT TESTED) |
| Emulasi Switch / PC | MODERATE–LIMITED (NOT TESTED) |
| Development (Python/Node/Git/SSH/C) | EXCELLENT (VERIFIED tools) |
| Local LLM ≤4B Q4 | GOOD (ESTIMATED) |
| Local LLM 7–8B Q4 | MODERATE, bersyarat (ESTIMATED) |
| Local LLM 12B+ | NOT RECOMMENDED |
| Storage lega | TIDAK — 88% terpakai (VERIFIED) |

---

## 2. Device Identity

| Item | Nilai | Status |
|---|---|---|
| Manufacturer | vivo | VERIFIED (device bridge) |
| Brand | iQOO | VERIFIED (device bridge) |
| Model | I2508 | VERIFIED (device bridge) |
| Device codename | I2508 | VERIFIED (device bridge) |
| Nama pasar | iQOO 15R | OFFICIAL (cocok dengan model I2508; rilis India 24 Feb 2026) |
| Product name | UNKNOWN (getprop tidak tersedia) | UNKNOWN |
| Android version | 17 | VERIFIED (`Build.VERSION.RELEASE` via bridge) |
| Android SDK | 37 | VERIFIED (`Build.VERSION.SDK_INT` via bridge) |
| Security patch | UNKNOWN (tidak dapat dibaca dari container) | UNKNOWN |
| Build number / display ID | UNKNOWN (tidak dapat dibaca; tidak dikumpulkan) | UNKNOWN |
| CPU architecture | AArch64 / ARM64 | VERIFIED (`uname -m`) |
| ABI | arm64-v8a (setara) | DERIVED (userland aarch64 + kernel aarch64) |
| SoC platform | Qualcomm SM8845 (Snapdragon 8 Gen 5) | VERIFIED (soc_id 685 + family Snapdragon) + OFFICIAL |
| Serial / IMEI / MAC | **Tidak dikumpulkan (redacted by policy)** | — |

Catatan: perangkat dirilis dengan Android 16 / OriginOS 6 (OFFICIAL), namun saat audit terbaca **Android 17 (SDK 37)** → perangkat sudah menerima update OS besar. Versi OriginOS saat ini UNKNOWN (lihat §11).

---

## 3. SoC & CPU

### 3.1 Identitas SoC

| Item | Nilai | Status |
|---|---|---|
| SoC | Qualcomm Snapdragon 8 Gen 5 | VERIFIED + OFFICIAL |
| Part number | SM8845 | OFFICIAL (Qualcomm product brief; cocok dengan soc_id 685) |
| soc_id (sysfs) | 685 | VERIFIED (`/sys/devices/soc0/soc_id`) |
| Family (sysfs) | Snapdragon | VERIFIED (`/sys/devices/soc0/family`) |
| Fabrikasi | TSMC 3 nm (N3P) | OFFICIAL |
| CPU | Qualcomm Oryon V3 (generasi ke-3), 8 core / 8 thread | VERIFIED (cpuinfo) + OFFICIAL |
| NPU | Hexagon (6 scalar + 8 vector HVX + 1 tensor HMX); zona termal `nsphvx-*`/`nsphmx-*` terbaca | OFFICIAL + VERIFIED (nama zona termal) |
| Modem | Snapdragon X80 5G (Sub-6 + mmWave capable; varian perangkat Sub-6) | OFFICIAL |
| ISP | Spectra, triple 20-bit AI-ISP | OFFICIAL |
| Chip tambahan | iQOO Q2 "Supercomputing Chip" (super-resolution & frame interpolation game) | OFFICIAL |

### 3.2 Konfigurasi core (VERIFIED dari `/proc/cpuinfo` + cpufreq)

| Cluster | Core | Tipe | Max hardware | Min | Governor |
|---|---|---|---|---|---|
| policy0 | cpu0–cpu5 (6 core) | Oryon V3 Performance ("Phoenix M", r5p0) | **3321.6 MHz** (`cpuinfo_max_freq`) | 556.8 MHz | walt |
| policy6 | cpu6–cpu7 (2 core) | Oryon V3 Prime ("Phoenix L", r4p0) | **3801.6 MHz** (`cpuinfo_max_freq`) | 806.4 MHz | walt |

- `CPU implementer: 0x51` (Qualcomm), `CPU part: 0x002`, variant `0x5` (Performance) / `0x4` (Prime) — VERIFIED.
- Semua 8 core online (`/sys/devices/system/cpu/online: 0-7`) — VERIFIED.
- Cache (OFFICIAL): L2 4 MB (klaster Prime) + 12 MB (klaster Performance); L3/SLC 6 MB.
- Tidak ada core efisiensi kecil — seluruh 8 core adalah core performa (arsitektur Oryon).

**Tabel frekuensi tersedia (VERIFIED, `scaling_available_frequencies`):**

- policy0 (MHz): 384, 556.8, 691.2, 806.4, 902.4, 1036.8, 1152, 1267.2, 1401.6, 1555.2, 1651.2, 1747.2, 1900.8, 2016, 2188.8, 2265.6, 2361.6, 2476.8, 2611.2, 2707.2, 2841.6, 3033.6, 3129.6, 3321.6
- policy6 (MHz): 384, 556.8, 806.4, 921.6, 1017.6, 1152, 1248, 1382.4, 1497.6, 1651.2, 1862.4, 1996.8, 2112, 2227.2, 2476.8, 2688, 2841.6, 2995.2, 3091.2, 3244.8, 3436.8 (+ boost 3532.8/3648/3724.8/3801.6 terlihat di `time_in_state`)

### 3.3 ⚠️ Cap frekuensi aktif saat audit (VERIFIED)

| Policy | `scaling_max_freq` saat audit | Max hardware | Rasio |
|---|---|---|---|
| policy0 (6×Performance) | 1900.8 MHz | 3321.6 MHz | ±57% |
| policy6 (2×Prime) | 1651.2 MHz | 3801.6 MHz | ±43% |

- Ini adalah **batas kebijakan daya/termal yang sedang aktif**, BUKAN batas hardware. `stats/time_in_state` membuktikan core Prime pernah berjalan di 3801.6 MHz (2410 jiffies) dan seluruh tangga frekuensi pernah dipakai.
- Penyebab pasti UNKNOWN dari container (kemungkinan: kebijakan daya OriginOS/scheduler Android, kondisi termal, atau mode performa non-Monster). cgroup proses terbaca `cpuset:/top-app`.
- **Implikasi untuk agent lain:** benchmark yang dijalankan dari environment ini bisa jauh di bawah potensi puncak perangkat. Jangan menyimpulkan performa hardware dari satu benchmark di container.

### 3.4 Instruction set & fitur CPU (VERIFIED dari `/proc/cpuinfo` Features)

`fp asimd evtstrm aes pmull sha1 sha2 crc32 atomics fphp asimdhp cpuid asimdrdm jscvt fcma lrcpc dcpop sha3 sm3 sm4 asimddp sha512 sve asimdfhm dit uscat ilrcpc flagm ssbs sb paca pacg dcpodp sve2 sveaes svepmull svesha3 svesm4 flagm2 frint svei8mm svebf16 i8mm bf16 rng bti ecv afp rpres sme smei8i32 smef16f32 smeb16f32 smef32f32 lrcpc3`

Poin penting:
- **ARMv9-A class** dengan **SVE2**, **SME/SME2** (`sme`, `smei8i32`, `smef16f32`, `smeb16f32`, `smef32f32`), **i8mm**, **bf16**, **RNG**, **BTI**, **PAC** — VERIFIED. (Sumber pihak ketiga menyebut ARMv9.2-A; versi minor persis ESTIMATED.)
- **SME + i8mm + bf16 = akselerasi matriks hardware di CPU** → sangat menguntungkan inference LLM (llama.cpp/KleidiAI) dan workload AI numerik. Ini pembeda utama vs Snapdragon generasi lama.
- Kriptografi lengkap (AES/PMULL/SHA1/SHA2/SHA3/SM3/SM4) → TLS, hashing, enkripsi cepat.

### 3.5 Analisis potensi (ESTIMATED, berdasarkan kelas SoC; bukan benchmark perangkat)

| Workload | Penilaian | Alasan |
|---|---|---|
| Single-core | Sangat kuat (kelas flagship 2026) | Oryon V3 Prime 3.8 GHz; Geekbench 6 referensi agregat ±2.900 (OFFICIAL pihak ketiga, NOT TESTED di perangkat ini) |
| Multi-core | Sangat kuat | 8 core performa penuh; referensi agregat ±9.500–9.600 GB6 multi |
| Multitasking | Sangat baik | 8 core besar + 12 GB RAM |
| Gaming | Excellent | CPU tidak akan menjadi bottleneck game Android |
| Emulasi | Sangat baik untuk PS2/GC/Wii; cukup untuk Switch | Single-core kuat + SVE2 |
| Development | Excellent | Compile C/C++/Python/Node native ARM64 cepat |
| AI inference CPU | Excellent untuk kelas mobile | SME/i8mm/bf16 + 8 core besar |

---

## 4. GPU

| Item | Nilai | Status |
|---|---|---|
| GPU model | **Adreno 829** | VERIFIED (`/sys/class/kgsl/kgsl-3d0/gpu_model: Adreno829`) |
| Manufacturer | Qualcomm | VERIFIED |
| Arsitektur | Adreno 800 series, "sliced architecture" (8 slice/CU, 1024 shader) | OFFICIAL |
| Clock maksimum | ±1225 MHz | OFFICIAL |
| Compute | ±2.5 TFLOPS FP32 / ±5.0 TFLOPS FP16 | ESTIMATED (angka pihak ketiga berbeda; orde besaran sama) |
| Vulkan | 1.3 (Qualcomm resmi; sebagian sumber menyebut 1.4) | OFFICIAL |
| OpenGL ES | 3.2 | OFFICIAL |
| OpenCL | 3.0 (full profile) | OFFICIAL |
| Driver version | UNKNOWN (tidak terbaca dari container) | UNKNOWN |
| GPU busy saat sampel | ±33.6% (`gpubusy: 336309 1001012`) | DERIVED |
| Zona termal GPU | 8 zona `gpuss-0..7` (konsisten arsitektur sliced) | VERIFIED |
| Ekstensi GPU | UNKNOWN (butuh aplikasi diagnostik OpenGL/Vulkan) | UNKNOWN |

**Analisis kompatibilitas (ESTIMATED / NOT TESTED):**
- **Android gaming:** kelas atas; semua game mobile 2026 berjalan pada setting tinggi (ESTIMATED).
- **GameHub / Winlator (PC emulation):** Adreno adalah GPU dengan dukungan driver Turnip (Mesa) terbaik di Android, namun Adreno 829 adalah generasi **a8xx** — dukungan Turnip untuk a8xx lebih baru dan masih matang bertahap; wajib pakai build Turnip/Mesa terbaru yang bundled di GameHub/Winlator. NOT TESTED.
- **PS2/Switch emulation:** Vulkan 1.3 + driver Adreno = kombinasi ideal untuk AetherSX2/NetherSX2 dan emulator Switch berbasis Vulkan. NOT TESTED.
- **Local AI acceleration:** OpenCL 3.0 + Vulkan 1.3 dapat dipakai llama.cpp (backend Vulkan), MLC LLM, ONNX Runtime (EP tertentu). NPU Hexagon hanya dapat diakses lewat jalur resmi Android (QNN/NNAPI/LiteRT), bukan dari container ini.

---

## 5. RAM & Memory

Sumber: `/proc/meminfo` + `free -h` (VERIFIED, snapshot 11:35 UTC).

| Item | Nilai | Status |
|---|---|---|
| RAM fisik (varian) | **12 GB LPDDR5X Ultra** | DERIVED (MemTotal) + OFFICIAL (varian 12 GB) |
| MemTotal | 11.508.944 kB ≈ **10.98 GiB** | VERIFIED |
| MemAvailable | 3.685.652 kB ≈ **3.52 GiB** | VERIFIED |
| MemFree | 623.860 kB ≈ 0.60 GiB | VERIFIED |
| Cached | 2.586.100 kB ≈ 2.47 GiB | VERIFIED |
| Buffers | 1.980 kB | VERIFIED |
| SwapTotal | 12.582.908 kB ≈ **12 GiB** | VERIFIED |
| SwapFree | 7.500.924 kB ≈ 7.15 GiB (terpakai ±4.85 GiB) | VERIFIED |
| Jenis swap | Kemungkinan ZRAM; `/proc/swaps` & `/sys/block/zram0/*` **Permission denied** | ESTIMATED |
| Kecepatan RAM | LPDDR5X Ultra 9600 Mbps, bandwidth ±76.8 GB/s | OFFICIAL |
| Memory expansion (virtual RAM vivo) | UNKNOWN — tidak dapat dibedakan dari ZRAM tanpa akses `/proc/swaps` | UNKNOWN |
| Memory pressure (PSI) | UNKNOWN (`/proc/pressure` tidak diperiksa/dibatasi) | UNKNOWN |

**Penting — bedakan fisik vs virtual:**
- RAM **fisik 12 GB**; ±1 GiB dipotong reserved kernel/hardware → terlihat 10.98 GiB.
- Swap 12 GiB adalah **bukan RAM tambahan** — ia lambat (kompresi ZRAM atau UFS). Swap terpakai ±4.85 GiB menandakan tekanan memori moderat saat audit.

**Analisis (ESTIMATED):**

| Workload | Penilaian |
|---|---|
| Multitasking harian | Sangat baik (12 GB) |
| Gaming berat | Baik; game besar + background apps tetap muat |
| Termux/container dev | Baik; environment ini sendiri ringan |
| Local LLM | **Budget realistis ±4–6 GB untuk model** setelah Android + apps; tersedia 3.5 GiB saat audit tanpa menutup apps |
| Emulator | Baik; PS2/Switch tidak rakus RAM (≤4 GB umum) |

---

## 6. Storage

Sumber: `df -h` (VERIFIED) + device bridge `file-diagnostics` (VERIFIED, cross-check).

| Item | Nilai | Status |
|---|---|---|
| Kapasitas terlihat (rootfs/userdata) | **218 GiB** (≈234 GB) | VERIFIED |
| Terpakai | 191 GiB (**88%**) | VERIFIED |
| Tersedia | **27 GiB** (bridge Android: 28.749.824.000 B ≈ 26.8 GiB) | VERIFIED |
| Varian perangkat | **256 GB** | DERIVED (218 GiB userdata konsisten 256 GB dikurangi partisi sistem; varian 512 GB akan ±450 GiB) |
| Tipe storage | UFS 4.1 | OFFICIAL |
| Filesystem | UNKNOWN (`/proc/mounts` dimask container; umumnya F2FS pada vivo) | UNKNOWN / ESTIMATED |
| Mount point | `/` (rootfs, rw) | VERIFIED |
| microSD | Tidak ada | OFFICIAL |
| USB OTG | Didukung | OFFICIAL |
| USB | Type-C, **USB 2.0** (480 Mbps) | OFFICIAL |

**Analisis:**
- ⚠️ **Storage adalah bottleneck utama saat ini** (88% penuh). UFS menurun performanya saat hampir penuh; sisakan ≥15–20%.
- Game Android besar (10–30 GB): muat 1–2 judul besar lagi.
- GameHub/PC games (5–50 GB/judul): sangat terbatas; pilih judul lama/ringan.
- ROM emulator: PS1/PSP/PS2/GC/Wii puluhan GB masih muat jika dibersihkan dulu.
- Model AI GGUF: 4B Q4 ±2.5 GB, 8B Q4 ±5 GB — muat beberapa model, tetapi jangan menumpuk.
- Dataset/dev: cukup untuk proyek kecil-menengah.

---

## 7. Display

Tidak ada node display yang dapat dibaca dari container (`getprop` tidak tersedia, `/sys/class/graphics` dibatasi). Data di bawah dari sumber resmi iQOO (OFFICIAL), dengan catatan perbedaan antar sumber.

| Item | Nilai | Status |
|---|---|---|
| Teknologi | AMOLED (LTPS), flat, 10-bit, 1 miliar warna | OFFICIAL |
| Ukuran | 6.59 inci (16.74 cm), ±90.5% screen-to-body | OFFICIAL |
| Resolusi | **2750 × 1260** ("1.5K"), rasio 19.5:9 | OFFICIAL |
| Kerapatan pixel | ±459 ppi | OFFICIAL (DERIVED dari resolusi+ukuran) |
| Refresh rate | Hingga **144 Hz**; panel native 120 Hz, 144 Hz pada game via interpolasi frame chip Q2 (sebagian sumber memasarkan "144Hz" langsung) | OFFICIAL (nuansa antar sumber berbeda) |
| Touch sampling | 360 Hz (gaming), **3200 Hz sesaat (instantaneous)** | OFFICIAL (iQOO store & rilis resmi) |
| HDR | HDR10+ | OFFICIAL |
| Kecerahan | 1800 nits HBM, 5000 nits peak (local) | OFFICIAL |
| PWM dimming | 4320 Hz (eye-care) | OFFICIAL |
| Gamut | 100% DCI-P3 | OFFICIAL |
| Proteksi | SCHOTT Xensation Alpha | OFFICIAL |
| `ro.sf.lcd_density` | UNKNOWN (tidak terbaca; ±480 dpi ESTIMATED dari 459 ppi) | UNKNOWN |
| Refresh rate aktif saat audit | UNKNOWN | UNKNOWN |

**Implikasi:** 144 Hz + touch sampling tinggi = unggul untuk game kompetitif (BGMI/MLBB/CoDM). Untuk emulator, target frame internal 60 fps sudah sangat cukup; resolusi 1260p memberi ruang upscale 2–3× tanpa melebihi layar.

---

## 8. Battery

| Item | Nilai | Status |
|---|---|---|
| Level saat audit | **90%** | VERIFIED (device bridge) |
| Tegangan | **4.261 V** | DERIVED (zona termal `vbat: 4261` mV) |
| Status charging | UNKNOWN (`/sys/class/power_supply/*` Permission denied) | UNKNOWN |
| Kesehatan baterai | UNKNOWN | UNKNOWN |
| Kapasitas | **7600 mAh** Si/C (silicon-anode, generasi ke-4) | OFFICIAL |
| Charging | **100W FlashCharge** (1→50% ±33 menit, 1→100% ±63 menit, klaim pabrikan) | OFFICIAL |
| Bypass charging | Ya — "Global Bypass Charging 2.0" (daya langsung ke sistem, baterai tidak ikut panas) | OFFICIAL |
| Reverse charging | Wired, hingga 7.5 W | OFFICIAL |
| Klaim daya tahan | ±8.14 jam gaming / 42.4 jam video (100%, kondisi uji pabrikan) | OFFICIAL (klaim, NOT TESTED) |

**Analisis (ESTIMATED):**
- 7600 mAh = salah satu baterai terbesar di kelasnya → sesi gaming panjang realistis.
- **Gaming while charging:** gunakan **bypass charging** (didukung resmi) untuk memangkas panas; ini fitur kunci perangkat ini.
- Risiko degradasi rendah bila tidak sering 0%/100% ekstrem; tidak ada stress test yang dilakukan dalam audit ini.

---

## 9. Thermal

Sumber: `/sys/class/thermal/thermal_zone*/{type,temp}` — **63 zona terbaca** (VERIFIED, snapshot 11:35 UTC; skin turun ke 39.5°C pada 11:40).

| Kelompok | Zona | Suhu snapshot |
|---|---|---|
| CPU cluster 0 | `cpu-0-*`, `cpullc-0-*`, `qmx-0-*` | 49.6–53.9°C |
| CPU cluster 1 (Prime) | `cpu-1-*`, `cpullc-1-*`, `qmx-1-*` | 49.6–53.7°C |
| GPU | `gpuss-0..7` | 47.5–49.0°C |
| DDR | `ddr` | 51.9°C |
| NPU/Hexagon | `nsphvx-0..2`, `nsphmx-0..1` | 48.4–49.6°C |
| Modem | `mdmss-0..3` | 46.9–48.0°C |
| Kamera / video | `camera-0/1`, `video` | 48.4–49.2°C |
| Skin (permukaan) | `sys-therm-0` | **41.8°C → 39.5°C** |
| USB port | `usb` | 37.0°C |
| PMIC | `pmr735d_tz` | 37.0°C |
| Hardware trip CPU | `cpu-hw-trip-0/1` | **105°C** (batas proteksi hardware) |
| BCL (battery current limit) | `pm8550-bcl-lvl0..2`, `pmiv010x-*` | 0 (tidak aktif saat sampel) |

Sistem pendingin (OFFICIAL): **6.5K IceCore VC** — vapor chamber 6500 mm², total area disipasi >10.000 mm² (graphite ganda + gel). Klaim: penurunan suhu inti CPU hingga 15°C.

**Analisis (ESTIMATED):**
- Snapshot menunjukkan perangkat **hangat (±50–54°C inti) meski beban ringan** — kemungkinan baru selesai beban kerja atau suhu lingkungan tinggi. Ini indikasi throttling akan muncul pada sesi gaming/AI panjang.
- Cap frekuensi aktif (§3.3) konsisten dengan mitigasi daya/termal yang sedang bekerja.
- Proteksi hardware 105°C + BCL aktif menunjukkan desain termal serius; **jangan pernah mematikan proteksi termal** (tidak dilakukan, tidak direkomendasikan).
- Tidak ada stress test ekstrem dilakukan (sesuai batasan audit).

---

## 10. Network & Connectivity

Akses lokal terbatas: `/proc/net/dev`, `/sys/class/net/*` → **Permission denied**; `ip` tidak tersedia. Data kemampuan dari sumber resmi (OFFICIAL). **Tidak ada IP/MAC/IMEI/SSID/password yang dikumpulkan.**

| Item | Nilai | Status |
|---|---|---|
| Wi-Fi | **Wi-Fi 7** (802.11 a/b/g/n/ac/ax/be), FastConnect 7900 | OFFICIAL |
| Band Wi-Fi | 2.4 GHz + 5 GHz (6 GHz tergantung regulasi/varian) | OFFICIAL / UNKNOWN untuk 6 GHz lokal |
| Bluetooth | **6.0** (A2DP, LE) | OFFICIAL |
| Seluler | 5G SA/NSA Sub-6, dual nano-SIM, 4G LTE penuh | OFFICIAL |
| Band 5G (varian India) | SA: n1/n3/n5/n7/n8/n18/n20/n26/n28A/n28B/n38/n40/n41/n48/n75/n71/n77/n78; NSA: subset | OFFICIAL |
| Modem | Snapdragon X80 (DL hingga 10 Gbps teoritis) | OFFICIAL |
| NFC | Ya | OFFICIAL |
| IR blaster | Ya | OFFICIAL |
| GNSS | GPS (L1+L5), GLONASS, Galileo, BeiDou, QZSS, NavIC | OFFICIAL |
| USB | Type-C USB 2.0 + OTG | OFFICIAL |
| Hotspot | Didukung (fitur standar Android) | ESTIMATED |
| Audio | Dual stereo speakers; tanpa jack 3.5 mm (audio via Type-C) | OFFICIAL |
| Status koneksi aktual | UNKNOWN (interface tidak terbaca dari container) | UNKNOWN |
| IP / MAC | **Tidak dikumpulkan (masked by policy)** | — |

**Implikasi:** Wi-Fi 7 + BT 6.0 = latensi rendah untuk cloud gaming & transfer lokal; 5G Sub-6 cukup untuk streaming/model download. USB 2.0 membatasi transfer kabel besar (±30–40 MB/s realistis) — andalkan Wi-Fi untuk file besar.

---

## 11. Android & OriginOS

| Item | Nilai | Status |
|---|---|---|
| Android version | **17** | VERIFIED (bridge) |
| SDK | **37** | VERIFIED (bridge) |
| OriginOS saat rilis | OriginOS 6 (berbasis Android 16) | OFFICIAL |
| OriginOS saat ini | UNKNOWN — perangkat kini Android 17, menandakan update besar (kemungkinan OriginOS generasi berikutnya); cek Settings → About phone | UNKNOWN |
| Security patch | UNKNOWN — cek Settings → About phone → Security patch | UNKNOWN |
| Build number | UNKNOWN (tidak dikumpulkan) | UNKNOWN |
| Kebijakan update | 4 tahun upgrade OS + 6 tahun security patch | OFFICIAL |
| SELinux | UNKNOWN (`/sys/fs/selinux/enforce` Permission denied); default Android = enforcing | UNKNOWN / ESTIMATED enforcing |
| Root status | Tidak ada indikasi root perangkat; environment agent adalah container terkurung (lihat §19) | DERIVED |
| Bootloader | UNKNOWN (`ro.boot.*` tidak dapat dibaca); vivo umumnya locked & tidak membuka unlock resmi | UNKNOWN |
| Developer options / USB debugging | UNKNOWN (tidak dapat dibaca; ADB tidak digunakan sesuai batasan) | UNKNOWN |

---

## 12. Kernel & Architecture

| Item | Nilai | Status |
|---|---|---|
| Kernel (uname -a) | **6.12.58-android16-6** … `-4k`, `#1 SMP PREEMPT`, build 2026-07-27 | VERIFIED |
| GKI | Android Generic Kernel Image generasi android16 (6.12), **page size 4K** (suffix `-4k`) | VERIFIED + DERIVED |
| Build ID kernel | Disamarkan sebagian (`g09eda8c…`, `ab…`) sesuai kebijakan redaksi | VERIFIED (redacted) |
| Arsitektur kernel | aarch64 | VERIFIED |
| Userland | Ubuntu 24.04.3 LTS ARM64 (aarch64-unknown-linux-gnu) | VERIFIED (`/etc/os-release`) |
| ⚠️ `/proc/version` | Dimask oleh container menjadi `6.6.0-qroot` — **jangan dipakai**; `uname -a` adalah sumber benar | VERIFIED (perbedaan terdokumentasi) |
| `/proc/stat`, `/proc/loadavg`, `/proc/uptime` | Dimask (semua nol) oleh layer container → utilisasi CPU & uptime **tidak dapat diukur dari sini** | VERIFIED (keterbatasan) |
| PID namespace | Terisolasi (hanya proses container terlihat) | DERIVED |

---

## 13. Termux Environment

**Termux TIDAK terdeteksi.** `termux-info`, `pkg`, `getprop`, `ip` tidak ada. Environment aktual adalah **container Ubuntu 24.04.3 LTS bawaan aplikasi OpenChamber** (layer "qroot"), berjalan di atas Android 17 host, dengan device-capability bridge HTTP loopback (health: OK).

| Item | Nilai | Status |
|---|---|---|
| Environment | Ubuntu 24.04.3 LTS (Noble) ARM64 container | VERIFIED |
| Shell | bash 5.2.21 (+ sh) | VERIFIED |
| Prefix | `/usr` (standar Ubuntu, bukan `/data/data/com.termux/files/usr`) | VERIFIED |
| Home | `/root` (di dalam container) | VERIFIED |
| Filesystem | read-write (`rootfs rw`) | VERIFIED |
| Package manager | `apt` / `apt-get` / `dpkg` tersedia (**tidak ada instalasi dilakukan**) | VERIFIED |
| Akses `/proc` | Parsial (cpuinfo/meminfo nyata; stat/uptime/loadavg dimask) | VERIFIED |
| Akses `/sys` | Parsial (cpufreq & thermal luas; block/power_supply/kgsl-list dibatasi) | VERIFIED |
| Android API | Via device bridge (device-info, battery, notify, clipboard, file share/save/pick) | VERIFIED |
| Native ARM64 | Ya — seluruh userland aarch64 | VERIFIED |
| Confinement | uid 0 container, `CapEff=0`, `NoNewPrivs=1`, `Seccomp=2` (7 filter) | VERIFIED |

**Untuk agent lain:** panduan berbasis Termux (`pkg install …`) tidak berlaku langsung; padanannya `apt install …` (dengan persetujuan user). Path Android shared storage tidak otomatis ter-mount; gunakan file bridge (`file-save`/`file-pick`) untuk impor/ekspor.

---

## 14. Development Capability

Tool terdeteksi & versi (VERIFIED):

| Tool | Versi | Status |
|---|---|---|
| Python | 3.12.3 (`/usr/bin/python3`, `/usr/local/bin/python`) + pip 24.0 | VERIFIED |
| Node.js | v22.23.2 | VERIFIED |
| npm / npx / corepack | 10.9.8 | VERIFIED |
| Git | 2.43.0 | VERIFIED |
| SSH | OpenSSH 9.6p1 (OpenSSL 3.0.13) | VERIFIED |
| GCC / G++ | 13.3.0 | VERIFIED |
| make | tersedia | VERIFIED |
| curl | 8.5.0 (HTTP2/3 libs, brotli, zstd) | VERIFIED |
| wget | 1.21.4 | VERIFIED |
| bash | 5.2.21 | VERIFIED |
| vim, zip, unzip, tar, sqlite3 | tersedia | VERIFIED |
| cloudflared | tersedia (`/usr/local/bin`) | VERIFIED |
| OpenCode / OpenChamber CLI | tersedia | VERIFIED |
| clang, cmake, go, rustc, java, php, tmux, nano, htop, rsync, ffmpeg | **TIDAK terdeteksi** (dapat ditambah via apt dengan persetujuan) | VERIFIED (absen) |

Total ±668 biner di `/usr/bin`.

|Kapabilitas | Penilaian | Catatan |
|---|---|---|
| Python development | EXCELLENT | 3.12 + pip; wheels aarch64 (numpy, onnxruntime, llama-cpp-python, dll.) tersedia |
| Node.js / web dev | EXCELLENT | Node 22 LTS + npm; dev server lokal via localhost |
| API development | EXCELLENT | curl/ssh/cloudflared untuk tunneling; Python/Node server |
| C/C++ build | GOOD | GCC 13.3 + make; clang/cmake perlu instalasi (approval) |
| Git & SSH workflow | EXCELLENT | lengkap |
| AI coding agents | EXCELLENT | OpenCode sendiri berjalan native di sini |
| Android app build (Gradle/SDK) | NOT AVAILABLE | tanpa Java/Android SDK; tidak realistis di container ini |
| Docker/KVM/perf | NOT AVAILABLE | seccomp + tanpa capabilities; jangan direkomendasikan |
| ARM64 compatibility | EXCELLENT | seluruh stack native aarch64 |

---

## 15. Gaming Capability

Basis: SD 8 Gen 5 + Adreno 829 + 12 GB + 144 Hz + Q2 chip + VC 6500 mm² (VERIFIED/OFFICIAL). Penilaian performa = **ESTIMATED, NOT TESTED** (tanpa benchmark langsung).

| Aspek | Penilaian | Catatan |
|---|---|---|
| CPU untuk game | EXCELLENT | 8 core Oryon V3; tidak ada bottleneck CPU |
| GPU untuk game | EXCELLENT (kelas atas) | Adreno 829; di bawah 8 Elite Gen 5 (Adreno 840) namun sangat kuat |
| RAM untuk game | GOOD–EXCELLENT | 12 GB cukup untuk game terberat + background |
| Display | EXCELLENT | 144 Hz (interpolasi Q2), touch 360/3200 Hz |
| Thermal | GOOD (dengan manajemen) | VC besar; snapshot audit hangat → sesi panjang perlu bypass charging & casing dilepas |
| Storage | **LIMITED saat ini** | hanya 27 GB bebas untuk game baru |
| Fitur gaming resmi | Monster Mode, Monster HyperCore Engine, Q2 super-resolution 1.5K + 144 FPS interpolation (BGMI), predictive scheduling | OFFICIAL |

**Kesimpulan:** perangkat ini dirancang untuk gaming berat (BGMI/PUBG, Genshin Impact, Honkai: Star Rail, Zenless Zone Zero, Wuthering Waves, CoDM, MLBB) pada setting tinggi. AnTuTu klaim pabrikan ±3.59 juta (OFFICIAL klaim, NOT TESTED). Jangan menjanjikan FPS spesifik tanpa benchmark.

---

## 16. GameHub Compatibility

GameHub (Windows-game-on-Android: Box64 + Wine + DXVK/Turnip) — **NOT TESTED** di perangkat ini; analisis berdasar hardware:

| Faktor | Kondisi | Dampak |
|---|---|---|
| CPU (Box64) | Oryon V3 3.8 GHz, 8 core | Sangat baik untuk translasi x86→ARM |
| GPU | Adreno 829 (a8xx) | Turnip a8xx tersedia di build Mesa terbaru; pastikan GameHub memakai Turnip ter-update, bukan driver Qualcomm bawaan |
| Vulkan | 1.3 | DXVK (DX9/10/11) didukung; VKD3D (DX12) terbatas di mobile |
| RAM | 12 GB (tersedia ±3.5–6 GB) | Cukup untuk game PC era ≤2015 dan indie |
| Storage | 27 GB bebas | **Pembatas utama** — game PC besar tidak muat |
| Thermal | VC besar, snapshot hangat | Sesi PC-emulation panjang → throttling mungkin |

**Estimasi (ESTIMATED):**
- Game DX9/DX11 ringan–menengah (Skyrim, GTA IV/V setting rendah, Need for Speed lama, indie): **MODERATE–GOOD**, playable dengan tuning.
- Game DX12 AAA modern: **LIMITED / NOT RECOMMENDED**.
- Selalu mulai dari preset rendah, resolusi 720p, Turnip terbaru, dan uji per game.

---

## 17. Emulator Capability

Semua baris **NOT TESTED** pada perangkat ini; estimasi berdasar kelas SoC (setara Snapdragon 8 Elite generasi sebelumnya) + Adreno 829 + 12 GB RAM.

| Emulator | Capability | Performance Estimate | Recommended Settings | Limitation |
|---|---|---|---|---|
| PS1 (DuckStation/ePSXe) | EXCELLENT | Full speed, upscale 4–8× | Vulkan, 4× resolusi, PGXP opsional | Praktis tidak ada |
| PSP (PPSSPP) | EXCELLENT | Full speed hampir semua judul, 3–5× resolusi | Vulkan, auto frameskip off, 3× render | Sebagian kecil judul sangat berat perlu 2× |
| PS2 (AetherSX2/NetherSX2) | GOOD–EXCELLENT | Mayoritas full speed 2–3× resolusi | Vulkan, 2× native, blending basic, MTVU on | Judul berat (God of War, MGS3, Gran Turismo 4) mungkin perlu 1–2× + tweak |
| GameCube (Dolphin) | EXCELLENT | Mayoritas full speed 2–3× IR | Vulkan, 2× IR, shader cache on | Beberapa judul efek khusus perlu akurat |
| Wii (Dolphin) | GOOD–EXCELLENT | Mayoritas full speed | Vulkan, 2× IR, motion via sensor perangkat | Judul berat (Mario Galaxy 2, Xenoblade) sesekali drop |
| Wii U (Cemu Android) | MODERATE | Banyak judul playable; belum merata | Vulkan, 720p output, async compile | Kematangan emulator Android masih berkembang; judul berat LIMITED |
| Switch (fork Yuzu/Sudachi/Citron, dll.) | MODERATE–GOOD | 2D & 3D ringan–menengah baik; AAA berat terbatas | Vulkan + Turnip terbaru, 0.75–1× resolusi, handheld mode | Driver a8xx masih matang; TotK/BotW kelas berat LIMIT; hanya gunakan dump game milik sendiri (legal) |
| 3DS (Azahar/Citra) | GOOD–EXCELLENT | Mayoritas full speed 2–3× | Vulkan/OpenGLES, 2× | Sebagian judul butuh akurat |
| PS Vita (Vita3K) | MODERATE | Banyak judul playable | Vulkan, resolusi 1× | Kompatibilitas per-game bervariasi |
| PC (Winlator/GameHub) | MODERATE | Lihat §16 | Box64 + Turnip + DXVK, preset rendah | Storage & DX12 |
| Arcade/MAME | EXCELLENT | Full speed | Default | Tidak ada |

**Catatan umum:** gunakan backend **Vulkan** bila tersedia; Adreno + Turnip adalah kombinasi terbaik. Untuk emulator Switch/PC, kebaruan driver Turnip menentukan hasil — perbarui hanya dari sumber resmi emulator.

---

## 18. Local AI & LLM

### 18.1 Fondasi hardware

| Faktor | Kondisi | Status |
|---|---|---|
| ARM64 + SVE2 + **SME/i8mm/bf16** | Akselerasi matriks CPU tersedia → llama.cpp (KleidiAI) sangat diuntungkan | VERIFIED |
| GPU OpenCL 3.0 / Vulkan 1.3 | Backend Vulkan llama.cpp, MLC LLM, ONNX Runtime | OFFICIAL |
| NPU Hexagon | Hanya via jalur Android resmi (QNN/LiteRT/NNAPI) dari aplikasi, bukan dari container | OFFICIAL |
| RAM fisik | 12 GB; tersedia saat audit ±3.5 GiB; budget realistis model ±4–6 GB setelah menutup background apps | VERIFIED + ESTIMATED |
| Storage | 27 GB bebas → beberapa model GGUF muat | VERIFIED |
| Runtime di environment ini | Python 3.12 + pip (llama-cpp-python, onnxruntime aarch64), Node 22; cmake/clang belum ada untuk build llama.cpp dari sumber | VERIFIED |

### 18.2 Tabel kelayakan model (ESTIMATED; NOT TESTED — tanpa benchmark on-device)

Asumsi: kuantisasi GGUF, konteks 4K, Android + apps memakan ±3–4 GB, tanpa swap-heavy.

| Model Size | Feasibility | Quantization | RAM Requirement | Performance (estimasi kasar CPU) |
|---|---|---|---|---|
| 0.5B–1B | EXCELLENT | Q4_K_M / Q8_0 | ±1–2 GB | ±20–40+ tok/s |
| 1B–3B | EXCELLENT | Q4_K_M | ±2–3 GB | ±15–30 tok/s |
| 3B–4B | GOOD (sweet spot) | Q4_K_M | ±3–4.5 GB | ±8–18 tok/s |
| 7B–8B | MODERATE (bersyarat: tutup background apps, konteks ≤4–8K) | Q4_K_M / IQ4_XS | ±5–6.5 GB | ±4–9 tok/s |
| 12B+ | LIMITED / NOT RECOMMENDED | IQ3_XXS / IQ4_XS (memaksa) | ±8–10+ GB | <±3–5 tok/s, risiko swap berat & OOM-kill Android |

Angka tok/s adalah **ESTIMATED orde besaran** untuk kelas SoC ini — bukan hasil ukur. KV cache bertambah seiring konteks; konteks panjang (≥16K) pada 7–8B dapat menambah 1–3 GB.

### 18.3 Tools

| Tool | Kelayakan | Catatan |
|---|---|---|
| llama.cpp | GOOD–EXCELLENT | Build ARM64 (butuh cmake/clang via apt + approval) atau `llama-cpp-python` wheel; backend CPU (SME) & Vulkan |
| MLC LLM | GOOD | Aplikasi Android resmi; GPU Adreno via OpenCL/Vulkan |
| Google AI Edge Gallery | GOOD | Aplikasi Android; model LiteRT GPU/CPU; perlu persetujuan instalasi |
| ONNX Runtime | GOOD | Wheel aarch64 via pip; CPU EP (+ NNAPI EP terbatas dari app) |
| GGUF ecosystem | EXCELLENT | Format utama rekomendasi |
| Local embeddings | EXCELLENT | all-MiniLM-L6-v2 / bge-small (GGUF/ONNX) <500 MB |
| Lightweight RAG | GOOD | Python + sqlite-vec/Chroma + embeddings lokal; muat nyaman di RAM |
| Local AI assistants | GOOD | OpenCode sendiri sudah berjalan di perangkat ini |

### 18.4 Rekomendasi model konkret (per akhir 2026)

- **Utama (keseimbangan):** `Qwen3-4B-Instruct-2507 Q4_K_M` atau `Gemma-3-4B-it-QAT Q4_0` (±2.5–3 GB).
- **Cepat/agent ringan:** `Qwen3-1.7B Q4_K_M`, `Llama-3.2-3B-Instruct Q4_K_M`.
- **Coding:** `Qwen2.5-Coder-3B-Instruct Q4_K_M` (atau Qwen3-4B).
- **Kualitas maksimum yang masih aman:** `Qwen3-8B Q4_K_M` / `Llama-3.1-8B-Instruct Q4_K_M` — hanya dengan background apps ditutup, konteks ≤4K.
- **Multimodal ringan:** `Gemma-3-4B-it` (vision via llama.cpp mtmd) atau `Moondream2`.
- **Embeddings/RAG:** `all-MiniLM-L6-v2` atau `bge-small-en-v1.5`.
- **Hindari:** model 12B+ dense, konteks sangat panjang pada 8B, dan model FP16.

---

## 19. Security Overview

| Item | Nilai | Status |
|---|---|---|
| Security patch | UNKNOWN (tidak terbaca; cek Settings) | UNKNOWN |
| SELinux | UNKNOWN (node dibatasi); default Android enforcing | UNKNOWN / ESTIMATED |
| Root perangkat | Tidak ada indikasi root; tidak ada upaya root dilakukan | DERIVED |
| Environment agent | uid 0 **di dalam container**, `CapEff=0000000000000000`, `CapBnd=0`, `NoNewPrivs=1`, `Seccomp=2` (7 filter), cgroup `apps/uid_10287` → **terkurung penuh, bukan root Android** | VERIFIED |
| Bootloader | UNKNOWN (umumnya vivo locked) | UNKNOWN |
| Enkripsi | UNKNOWN lokal; Android modern wajib File-Based Encryption | ESTIMATED (FBE aktif) |
| Developer options / USB debugging | UNKNOWN; ADB tidak digunakan | UNKNOWN |
| Update policy | 4 tahun OS + 6 tahun patch (mendukung postur keamanan jangka panjang) | OFFICIAL |
| Data sensitif | Serial, IMEI, MAC, IP, kredensial **tidak dikumpulkan**; build ID kernel disamarkan sebagian | Policy |
| Biometrik | Ultrasonic in-display fingerprint + face unlock | OFFICIAL |
| Ketahanan fisik | IP68 + IP69 | OFFICIAL |

Tidak ada perubahan konfigurasi keamanan, SELinux, partisi sistem, atau permission yang dilakukan.

---

## 20. Device Capability Matrix

| Capability | Status | Notes |
|---|---|---|
| Native Gaming | EXCELLENT (ESTIMATED) | SD 8 Gen 5 + Adreno 829 + 144 Hz + Q2 |
| Heavy Gaming | GOOD–EXCELLENT (ESTIMATED) | Throttling mungkin pada sesi panjang; bypass charging membantu |
| GameHub | MODERATE (NOT TESTED) | DX9/11 ringan–menengah; storage terbatas; Turnip a8xx terbaru wajib |
| PS2 Emulation | GOOD–EXCELLENT (NOT TESTED) | AetherSX2/NetherSX2 Vulkan 2× |
| PC Emulation | MODERATE–LIMITED (NOT TESTED) | Judul lama/indie; bukan AAA DX12 |
| Termux | NOT INSTALLED | Environment = Ubuntu 24.04 container OpenChamber (fungsional setara untuk dev) |
| Python | EXCELLENT (VERIFIED) | 3.12.3 + pip 24.0 |
| Node.js | EXCELLENT (VERIFIED) | v22.23.2 + npm 10.9.8 |
| Git | EXCELLENT (VERIFIED) | 2.43.0 |
| SSH | EXCELLENT (VERIFIED) | OpenSSH 9.6p1 |
| Web Development | EXCELLENT | Node/Python server lokal + cloudflared |
| API Development | EXCELLENT | curl, ssh tunnel, cloudflared |
| Local AI | GOOD–EXCELLENT | SME/i8mm CPU + Vulkan GPU; embeddings & RAG nyaman |
| Local LLM | GOOD ≤4B; MODERATE 7–8B; NOT RECOMMENDED 12B+ | Lihat §18 |
| RAG | GOOD | sqlite-vec/Chroma + embeddings lokal |
| Multitasking | EXCELLENT | 12 GB + 8 core besar |
| File Transfer | GOOD (nirkabel) / LIMITED (kabel) | Wi-Fi 7 cepat; USB 2.0 lambat; file bridge tersedia |

---

## 21. Bottleneck Analysis

| # | Bottleneck | Evidence | Impact | Severity | Recommended Action |
|---|---|---|---|---|---|
| 1 | **Storage hampir penuh** | `df -h`: 191G/218G terpakai (88%), sisa 27G (VERIFIED) | Game baru, model AI, ROM emulator terbatas; performa UFS menurun saat penuh | **HIGH** | Bersihkan media/cache, pindahkan file via Wi-Fi, jaga ≥15–20% bebas |
| 2 | **RAM tersedia rendah saat audit** | MemAvailable 3.52 GiB dari 10.98 GiB; swap terpakai ±4.85 GiB (VERIFIED) | LLM 7–8B berisiko OOM/swap; multitasking berat + AI bersamaan sulit | MODERATE | Tutup background apps sebelum inference; pilih model ≤4B Q4; konteks ≤4–8K |
| 3 | **Cap frekuensi CPU aktif** | `scaling_max_freq` 1900.8/1651.2 MHz vs hardware 3321.6/3801.6 MHz (VERIFIED) | Benchmark dari environment ini meremehkan perangkat; workload berat melambat saat cap aktif | MODERATE (observasional) | Jalankan beban via aplikasi foreground/Monster Mode; jangan simpulkan hardware dari benchmark container |
| 4 | **Termal snapshot hangat** | CPU 49.6–53.9°C, skin 41.8°C saat beban ringan (VERIFIED) | Throttling pada gaming/AI berkepanjangan | MODERATE | Bypass charging saat gaming, lepas casing, hindari matahari langsung, jeda sesi |
| 5 | **Pembatasan container (seccomp/caps/proc-mask)** | CapEff=0, Seccomp=2, `/proc/stat` nol, `/sys/block` & `power_supply` denied (VERIFIED) | Diagnostik dalam terbatas; Docker/KVM/perf tidak mungkin; beberapa software tidak jalan | LOW–MODERATE (informasional) | Gunakan aplikasi Android untuk diagnostik penuh; jangan rekomendasikan tool butuh privilege |
| 6 | **USB 2.0** | Spesifikasi resmi (OFFICIAL) | Transfer kabel besar lambat (±30–40 MB/s) | LOW | Andalkan Wi-Fi 7 / file bridge untuk file besar |
| 7 | **Kematangan driver Turnip a8xx** | Adreno 829 generasi baru (VERIFIED GPU; dukungan driver ESTIMATED) | Emulator Switch/PC bisa crash/glitch pada driver lama | MODERATE (NOT TESTED) | Pakai build emulator + Turnip terbaru; uji per game |
| 8 | **Bukan Termux** | `termux-info`/`pkg` absen (VERIFIED) | Panduan Termux tidak langsung berlaku; beberapa package Android-specific absen | LOW (informasional) | Gunakan apt + dokumentasi ini sebagai referensi environment |
| 9 | **Android background limits** | Platform Android modern (ESTIMATED) | Proses dev/AI panjang bisa dibunuh sistem bila app tidak foreground | LOW–MODERATE | Jalankan workload dalam app foreground service (seperti OpenChamber); battery optimization exclusion bila perlu |

---

## 22. Optimization Recommendations

Semua rekomendasi **tanpa root, tanpa modifikasi sistem berisiko**. Prioritas: 1 = tertinggi.

| # | Recommendation | Reason | Benefit | Risk | Priority | Root? |
|---|---|---|---|---|---|---|
| 1 | Bersihkan storage hingga ≥15–20% bebas (hapus media duplikat, cache app, pindahkan via Wi-Fi) | 88% penuh (VERIFIED) | Performa UFS stabil, ruang game/model AI | Rendah (pastikan backup sebelum hapus) | 1 | Tidak |
| 2 | Aktifkan **Monster Mode** via Game Space saat gaming berat | Fitur resmi iQOO untuk melepas batas performa | FPS lebih stabil | Panas & baterai meningkat (wajar) | 1 | Tidak |
| 3 | Gunakan **Bypass Charging 2.0** saat gaming sambil mengisi daya | Fitur resmi; daya langsung ke sistem | Panas turun drastis, baterai awet | Perlu charger 100W yang kompatibel | 1 | Tidak |
| 4 | Lepas casing & hindari permukaan panas saat sesi panjang | VC 6500 mm² butuh pembuangan panas lancar | Throttling berkurang | Rendah | 2 | Tidak |
| 5 | Tutup background apps sebelum local LLM; pilih ≤4B Q4_K_M, konteks ≤4–8K | MemAvailable hanya ±3.5 GiB (VERIFIED) | Inferensi stabil tanpa OOM | Rendah | 2 | Tidak |
| 6 | Untuk LLM: llama.cpp CPU (SME) atau Vulkan; mulai dari Qwen3-4B Q4_K_M | SME/i8mm terverifikasi; 12 GB RAM | Kualitas/kecepatan optimal di kelas perangkat | Rendah; model salah pilih → lambat | 2 | Tidak |
| 7 | Emulator: backend Vulkan + Turnip terbaru; mulai resolusi 2× (PS2/GC) atau 1× (Switch) | Adreno 829 + Vulkan 1.3 | Kompatibilitas & performa terbaik | Driver lama → glitch (perbarui resmi) | 3 | Tidak |
| 8 | GameHub: preset rendah, 720p, DXVK (DX9/11), judul ≤2015 | Storage & batas translasi | Pengalaman playable realistis | Ekspektasi berlebihan pada AAA | 3 | Tidak |
| 9 | Dev: gunakan apt untuk menambah cmake/clang bila perlu build native (dengan persetujuan) | GCC ada, cmake/clang absen (VERIFIED) | Build llama.cpp/toolchain lengkap | Rendah; storage bertambah | 4 | Tidak |
| 10 | Multitasking: kunci app penting di recents; hindari RAM expansion saat storage penuh | Swap sudah 12 GiB & storage 88% | Tidak memperparah tekanan storage | RAM expansion memakai UFS yang sudah penuh | 4 | Tidak |
| 11 | Jaringan: pakai Wi-Fi 5/6/7 band 5 GHz untuk gaming & transfer besar | Wi-Fi 7 tersedia (OFFICIAL); USB 2.0 lambat | Latensi rendah, transfer cepat | Rendah | 5 | Tidak |
| 12 | Baterai: jaga 20–90% untuk pemakaian harian; hindari panas ekstrem saat charging | Baterai Si/C 7600 mAh | Umur baterai panjang | Rendah | 5 | Tidak |
| 13 | Diagnostik lanjutan (patch level, OriginOS, refresh rate aktif): cek Settings → About phone / Display | Node sistem dibatasi dari container (VERIFIED) | Melengkapi data UNKNOWN dokumen ini | Tidak ada | 5 | Tidak |

**Dilarang / tidak direkomendasikan:** root, unlock bootloader, ubah governor/thermal via sysfs, "tweak" build.prop, menghapus proteksi termal, aplikasi "RAM booster" pihak ketiga, stress test ekstrem.

---

## 23. Known Limitations

1. **Bukan Termux** — environment adalah container Ubuntu OpenChamber; path/permission berbeda dari Termux asli.
2. **Container terkurung** — tanpa capabilities, seccomp aktif; tidak bisa mengakses hardware Android secara penuh (GPU/NPU langsung, sensor, display).
3. **`/proc` dimask sebagian** — utilisasi CPU, load, uptime tidak terukur dari sini; `/proc/version` palsu (`6.6.0-qroot`).
4. **`/sys` dibatasi sebagian** — block device, power_supply, listing kgsl/devfreq/misc, SELinux enforce tidak terbaca.
5. **`getprop` tidak tersedia** — build ID, security patch, lcd_density, status bootloader/ADB tidak dapat dibaca.
6. **Tidak ada benchmark langsung** — semua angka performa adalah estimasi kelas SoC; cap frekuensi aktif (§3.3) membuat benchmark container tidak representatif.
7. **Storage 88% penuh** — membatasi rekomendasi game/model besar sampai dibersihkan.
8. **Emulator & GameHub belum diuji** pada perangkat ini (NOT TESTED).
9. **NPU tidak dapat diakses** dari container; hanya via aplikasi Android dengan SDK resmi.
10. **Data display/battery detail** (refresh aktif, status charging, health) UNKNOWN dari environment ini.

---

## 24. Audit Methodology

### 24.1 Prinsip
- Native on-device, **tanpa ADB/PC/scrcpy/remote shell**, tanpa root, tanpa instalasi aplikasi/package, tanpa perubahan konfigurasi, read-only.
- Deteksi command sebelum dipakai (`command -v …`); akses ditolak → dicatat, **tidak dibypass**.
- Data sensitif (serial, IMEI, MAC, IP, kredensial) tidak dikumpulkan; build ID kernel disamarkan sebagian.

### 24.2 Command yang digunakan (berhasil)

```bash
# Environment detection
uname -a
command -v bash sh python python3 node npm git php ssh clang curl wget pkg termux-info getprop ip free df
cat /etc/os-release
id; cat /proc/self/status   # Seccomp/CapEff/NoNewPrivs
cat /proc/self/cgroup

# Device bridge (aplikasi host, loopback terautentikasi)
python3 "$OPENCODE_CONFIG_HOME/skills/android-device/android_device.py" health
python3 "$OPENCODE_CONFIG_HOME/skills/android-device/android_device.py" device-info
python3 "$OPENCODE_CONFIG_HOME/skills/android-device/android_device.py" tools
python3 "$OPENCODE_CONFIG_HOME/skills/android-device/android_device.py" file-diagnostics

# CPU / SoC
cat /proc/cpuinfo
ls /sys/devices/system/cpu/
cat /sys/devices/system/cpu/online /sys/devices/system/cpu/present
cat /sys/devices/soc0/family /sys/devices/soc0/soc_id
# per-cpu: scaling_max_freq, scaling_min_freq, scaling_cur_freq, scaling_governor, cpuinfo_max_freq
cat /sys/devices/system/cpu/cpufreq/policy{0,6}/affected_cpus
cat /sys/devices/system/cpu/cpufreq/policy{0,6}/scaling_available_frequencies
head -25 /sys/devices/system/cpu/cpufreq/policy6/stats/time_in_state

# GPU
cat /sys/class/kgsl/kgsl-3d0/gpu_model
cat /sys/class/kgsl/kgsl-3d0/gpubusy

# Memory / storage
cat /proc/meminfo
free -h
df -h

# Thermal
ls /sys/class/thermal/
# loop: cat thermal_zone*/type + temp (63 zona)

# Runtime versions
python3 --version; node --version; npm --version; git --version; ssh -V
curl --version; wget --version; bash --version; gcc --version
python3 -m pip --version
ls /usr/bin | wc -l; ls /usr/local/bin

# Waktu audit
date -u
```

### 24.3 Command/akses yang GAGAL atau dibatasi (dicatat, tidak dibypass)

| Akses | Hasil |
|---|---|
| `getprop`, `termux-info`, `pkg`, `ip`, `php`, `clang`, `cmake`, `java`, `go`, `rustc`, `tmux`, `htop`, `ffmpeg` | Tidak tersedia (command not found) |
| `cat /proc/swaps` | Permission denied |
| `/sys/block/*` (termasuk `zram0/disksize`, `zram0/mm_stat`) | Permission denied |
| `/sys/class/power_supply/*` (battery capacity/status/voltage/temp/health) | Permission denied |
| `ls /sys/class/kgsl/`, `/sys/class/misc/`, `/sys/class/devfreq/` | Permission denied (file spesifik `gpu_model`/`gpubusy` tetap terbaca) |
| `ls /sys/devices/soc0/` | Permission denied (file `family`/`soc_id` terbaca) |
| `cat /sys/fs/selinux/enforce` | Permission denied |
| `cat /proc/net/dev`, `/sys/class/net/*` | Permission denied |
| `/proc/stat`, `/proc/loadavg`, `/proc/uptime` | Dimask container (nilai nol) |
| `/proc/version` | Dimask (`6.6.0-qroot`) — dipakai `uname -a` sebagai gantinya |
| `/proc/mounts` | Hanya `rootfs / rootfs rw` (dimask) |
| `/system/build.prop`, `/vendor/build.prop` | Tidak ada (di luar container) |
| `/sys/class/graphics/fb0/*` | Tidak terbaca |

### 24.4 Sumber resmi pelengkap (OFFICIAL)
- iQOO India/Global/Malaysia/TW — halaman produk & spesifikasi iQOO 15R (iqoo.com, mshop.iqoo.com)
- Qualcomm — Snapdragon 8 Gen 5 product page & product brief (SM8845)
- GSMArena — vivo iQOO 15R full specifications & berita peluncuran (24 Feb 2026)
- Peliputan peluncuran: Gadgets360, LiveMint, ETV Bharat (nuansa 120 Hz native + 144 Hz interpolasi Q2)

---

## 25. Data Reliability

Definisi label:
- **VERIFIED** — terbaca langsung dari perangkat saat audit.
- **OFFICIAL** — spesifikasi resmi manufacturer/Qualcomm (belum tentu identik dengan unit ini).
- **DERIVED** — dihitung dari data aktual perangkat.
- **ESTIMATED** — estimasi berbasis kelas hardware; bukan pengukuran.
- **UNKNOWN** — tidak dapat diakses secara sah dari environment ini.
- **NOT TESTED** — belum diuji (benchmark/game/emulator/model).

Ringkasan per domain:

| Domain | Dominasi label |
|---|---|
| Identitas device, Android/SDK | VERIFIED |
| SoC, CPU, frekuensi, governor, fitur ISA | VERIFIED (+ OFFICIAL untuk cache/fabrikasi) |
| GPU model | VERIFIED; clock/API OFFICIAL |
| RAM total/available/swap | VERIFIED; jenis swap ESTIMATED |
| Storage kapasitas/sisa | VERIFIED; varian 256 GB & UFS 4.1 DERIVED/OFFICIAL |
| Display | OFFICIAL (lokal UNKNOWN) |
| Battery level/tegangan | VERIFIED/DERIVED; kapasitas/charging OFFICIAL |
| Thermal | VERIFIED (snapshot) |
| Network | OFFICIAL (status lokal UNKNOWN) |
| Kernel | VERIFIED (sebagian redacted) |
| Environment & tools | VERIFIED |
| Gaming/emulator/AI performance | ESTIMATED / NOT TESTED |
| Security patch, SELinux, bootloader | UNKNOWN |

**Aturan untuk agent berikutnya:** jangan menaikkan label tanpa pengukuran baru; setiap benchmark/game/model yang benar-benar diuji harus memperbarui dokumen ini (tanggal + hasil + kondisi uji, termasuk status cap frekuensi §3.3).

---

## 26. Last Audit

| Item | Nilai |
|---|---|
| Tanggal | 2026-11-23 |
| Waktu | 11:32–11:40 UTC |
| Auditor | OpenCode agent (Qwen4 Max Free), native on-device |
| Metode | Read-only, tanpa ADB/root/instalasi/perubahan konfigurasi |
| Kondisi perangkat saat audit | Baterai 90%; CPU cap 1.90/1.65 GHz aktif; suhu inti ±50–54°C, skin 41.8→39.5°C; storage 88% terpakai; MemAvailable ±3.5 GiB |
| File | `DEVICE_INFO.md` (direktori kerja aktif sesi) |
| Status | Lengkap dengan keterbatasan terdokumentasi (§23); item UNKNOWN menunggu verifikasi manual via Settings atau audit lanjutan |
