# TESTING — protokol uji di HP

Uji dilakukan owner di iQOO 15R. Agent hanya mencatat hasil ke `memory/test-results.md`.

## Urutan uji V0.3 (pakai debug zip dulu)
1. **Load test:** import `xamonxx_naga8gen5V0.3-debug.zip` → driver muncul sebagai `xamonxx naga8gen5 V0.3 Debug` tanpa error.
2. **Cek log:** jalankan game 1 menit → `Download/xamonxx/` berisi `mode.txt` + `log_*.csv`.
   Catat bila folder tidak muncul (izin storage launcher = UNKNOWN).
3. **Sesi per mode (15 menit jalan-jalan kota di story mode):** set `mode.txt`, setting HP/launcher, dan profil command line sesuai
   tabel di `docs/INSTALL_AND_TUNING.md`:
   - `cool60` → `monster90` → `monster120` (istirahatkan HP ±10 menit di antara sesi supaya suhu awal mirip).
4. Kirim semua CSV ke PC (`dist/logs/`, tidak di-commit) → `python -I tools/analyze_log.py dist/logs/*.csv`.
5. Opsional: benchmark bawaan launcher (5 pass) dengan zip rilis sesuai mode, untuk dibandingkan dengan V0.2.

## Kondisi wajib dicatat
Baterai %, charging/bypass on/off, Monster Mode on/off, refresh rate layar, suhu ruangan (kira-kira), casing on/off.

## Yang dibaca dari log
- `gpu_floor_status`: `accepted` = KGSL menerima floor; `rejected` = floor tidak aktif (fps bisa lebih rendah dari harapan).
- `adpf_status`: `ok` / `ok-power-efficient` = hint CPU aktif; `unavailable` = Android menolak di proses ini.
- Persentase detik ≥ 95% target dan frame lambat/menit = ukuran "stabil".
- Tabel per 5 menit: apakah fps turun saat suhu naik (throttling).

## Template laporan (salin ke `memory/test-results.md`)
```markdown
### YYYY-MM-DD — V0.3 <mode> — GTA V
- Driver: xamonxx_naga8gen5V0.3-<debug|cool60|monster90|monster120>
- Launcher: versi …, Target FPS …, V-Sync …, resolusi …, profil command line …
- Kondisi: baterai …%, bypass …, Monster Mode …, layar … Hz, casing …, suhu ruangan …
- Ringkasan analyze_log.py: (tempel di sini)
- Kesimpulan: …
- Status: TESTED
```
