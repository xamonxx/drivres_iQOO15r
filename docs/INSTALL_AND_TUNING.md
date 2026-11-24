# INSTALL & TUNING — iQOO 15R (Adreno 829)

## Pasang driver
- Jangan extract zip. Kirim zip ke HP (Wi-Fi/kabel).
- **Launcher GTA V:** Settings → Import driver (.zip) → pilih zip.
- **GameHub:** pengaturan game → GPU Driver → Import / tambah driver lokal → pilih zip.
- **Winlator:** Container Settings → Graphics Driver → Install/Import → pilih zip.
- Cadangan bila crash: Turnip Gen8 V39 / V39 Patched (StevenMXZ), GameHub Rutabaga v3, atau `xamonxx_naga8gen5V0.1`.

## Pilih driver V0.4 sesuai mode
| Zip | Untuk | Isi di driver | Setelan HP | Launcher GTA V |
|---|---|---|---|---|
| `xamonxx_naga8gen5V0.4-cool60.zip` | 60 fps stabil, paling adem | tanpa limiter driver (pacing oleh V-Sync game), GPU floor 60%, hint ADPF 16.7 ms | Monster Mode **OFF**, layar 60 atau 120 Hz, bypass charging saat cas | **Target FPS 60, V-Sync ON (wajib)**, profil C |
| `xamonxx_naga8gen5V0.4-monster90.zip` | 90 fps | batas 90 fps + thermal guard (turun s/d 60 saat ≥95°C), GPU clock maks, hint ADPF | Monster Mode **ON**, layar 120/144 Hz | Target FPS Unlimited, V-Sync OFF, profil A, resolusi ±1280×586 |
| `xamonxx_naga8gen5V0.4-monster120.zip` | 120 fps (sesi pendek) | batas 120 fps + thermal guard (turun s/d 90 saat ≥95°C), GPU clock maks, hint ADPF | Monster Mode **ON**, layar 120/144 Hz | Target FPS Unlimited, V-Sync OFF, profil B, resolusi ±1280×586 |
| `xamonxx_naga8gen5V0.4-debug.zip` | uji & evaluasi | mode dari `mode.txt` + log CSV | sesuai mode yang diuji | sesuai mode yang diuji |

Profil command line: `docs/COMMANDLINE.md`.

### Pelajaran dari log V0.3 (2026-11-24)
- **CPU tertahan di 2.36 GHz (Performance) / 2.48 GHz (Prime)** selama main di semua mode — itu batas kebijakan daya
  OriginOS untuk app ini, bukan driver. ADPF aktif (`ok`) tapi tidak bisa melewati batas itu. Coba: tambahkan launcher
  GTA V (paket `com.thegwchr.fuzzy`) ke **Game Space** dan nyalakan **Monster Mode**, lalu cek kolom `cpu0_mhz` di log.
- **GPU 99% sibuk** di Monster90/120 → fps dibatasi kekuatan GPU, bukan driver. 90 fps stabil butuh setting/resolusi lebih ringan.
- **Monster120 terlalu panas untuk sesi panjang:** GPU 104°C, CPU 109°C dalam 2 menit. V0.4 menambah thermal guard.
- **Cool60 V0.3 tersendat** karena dua pembatas (limiter driver + V-Sync game) dan GPU floor 40% terlalu rendah. V0.4: limiter
  driver dimatikan untuk Cool60, floor naik ke 60%. **V-Sync ON + Target FPS 60 wajib** supaya tetap 60 dan adem.

### Batas kemampuan driver (jujur)
- **Clock CPU tidak bisa dipaksa oleh driver** (tanpa root = dilarang di project ini). Yang dilakukan driver Monster:
  minta "performance hint" resmi Android (ADPF) untuk thread render — Android yang memutuskan. Untuk CPU maksimal,
  nyalakan **Monster Mode** di Game Space. ADPF terbukti jalan di proses game (log V0.3: `ok`), tapi tetap tidak bisa
  melewati batas clock dari OS.
- **Driver tidak mengatur suhu.** Cool60 hanya mengurangi kerja sia-sia (tidak render > 60 fps). Thermal guard di Monster
  menurunkan batas fps saat CPU/GPU ≥95°C (lebih sedikit frame = lebih sedikit panas) dan menaikkannya lagi setelah 30 s ≤85°C.
  Throttling termal HP selalu tetap berlaku — pada sesi panjang fps Monster bisa turun karena panas.
- **Gambar tidak buriq** = resolusi + setting game (DOF off, FXAA off, anisotropic tinggi). Driver tidak menurunkan kualitas.
- 90 fps di layar 120 Hz tanpa V-Sync bisa sedikit tearing/judder; batas 90 dari driver menjaga jarak antar-frame tetap rata.

## Debug driver: buat log otomatis
1. Pasang `xamonxx_naga8gen5V0.4-debug.zip`, jalankan game sekali.
2. Buka file manager → `Download/xamonxx/`. Harus muncul `mode.txt` + `log_<tanggal>_<jam>_<mode>_<pid>.csv`.
   - Tidak muncul? Launcher tidak punya izin storage → log jatuh ke folder app (`/data/data/<paket>/files/xamonxx/`, perlu
     Shizuku/rish untuk diambil) atau hanya ke logcat (`logcat -s` dengan kata kunci `xamonxx`).
3. Ganti mode: edit baris pertama `mode.txt` → `cool60` / `monster90` / `monster120`, tutup game, buka ulang.
4. Kirim file CSV ke PC → `python -I tools/analyze_log.py <file.csv>` → ringkasan fps/suhu siap dicatat.
- Debug zip memakai flag build yang sama dengan rilis; bedanya hanya ada log 1 baris/detik (beban sangat kecil).
- Untuk main harian pakai zip rilis sesuai mode (tanpa log).

## GTA V (versi PC milik sendiri)
| Setting | Nilai awal |
|---|---|
| Versi game | **GTA V Legacy (DX11)** — hindari Enhanced (DX12) |
| Translation layer | DXVK 2.x |
| Resolusi | 1568×718 (Cool60/Monster90), ±1280×586 (Monster120) |
| Box64 preset | Performance |
| Pin threads to cores | ON |

Storage: GTA V ±95–110 GB, sisa HP ±27 GB (per 2026-11-23) → kosongkan dulu.

## Suhu adem (bukan fungsi driver)
- Pakai Cool60 + V-Sync ON + resolusi tidak berlebihan.
- **Bypass Charging 2.0** saat main sambil cas (`game_bypass_charge_support=1` terbaca di HP).
- Monster Mode hanya untuk Monster90/120 (lebih panas).
- Lepas casing, hindari main di bawah matahari / di kasur.
- Jangan pakai "thermal mod"/root — dilarang di project ini.
