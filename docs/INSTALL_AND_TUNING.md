# INSTALL & TUNING — iQOO 15R (Adreno 829)

## Pasang driver
- Jangan extract zip. Kirim `xamonxx_naga8gen5V0.x.zip` ke HP (Wi-Fi/kabel).
- **GameHub:** pengaturan game → GPU Driver → Import / tambah driver lokal → pilih zip.
- **Winlator:** Container Settings → Graphics Driver → Install/Import → pilih zip.
- Cadangan bila crash: Turnip Gen8 V39 / V39 Patched (StevenMXZ), GameHub Rutabaga v3.

## GTA V (versi PC milik sendiri)
| Setting | Nilai awal |
|---|---|
| Versi game | **GTA V Legacy (DX11)** — hindari Enhanced (DX12) |
| Translation layer | DXVK 2.x |
| Resolusi | 1280×720 |
| Box64 preset | Performance |
| Grafik in-game | Normal, MSAA off, FXAA on, grass Normal, reflection Normal |
| FPS cap | 30 (adem) / 40 (seimbang) |

Storage: GTA V ±95–110 GB, sisa HP ±27 GB (per 2026-11-23) → kosongkan dulu.

## Suhu adem (bukan fungsi driver)
- FPS cap 30–40, resolusi 720p.
- **Bypass Charging 2.0** saat main sambil cas.
- Monster Mode hanya bila perlu FPS maksimal (lebih panas).
- Lepas casing, hindari main di bawah matahari / di kasur.
- Jangan pakai "thermal mod"/root — dilarang di project ini.

## Target 90 / 120 fps (V0.2+)
FPS di atas 60 hanya mungkin bila **layar** juga di atas 60 Hz. Benchmark V0.1 menunjukkan `@ 60Hz`.
1. HP: Settings → Display → Screen refresh rate → tertinggi (120/144 Hz). Di Game Space, set frame rate game ini ke 120/144.
2. App GTA V: Target FPS → 90 atau 120 (atau Unlimited saat benchmark). V-Sync OFF untuk mengukur headroom.
3. Driver: mulai `V0.2-balanced`; bila masih drop, coba `V0.2-turbo` (lebih panas).
4. Bila 90 belum stabil: turunkan resolusi (1568×718 → ±1280×586), Graphics preset lebih rendah.
5. Panas: 90–120 fps **pasti** lebih panas dari 60. Pakai bypass charging, lepas casing. Driver tidak bisa membuat 120 fps sekaligus adem.
