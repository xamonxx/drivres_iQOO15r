# TESTING — protokol uji di HP

Uji dilakukan owner di iQOO 15R. Agent hanya mencatat hasil ke `memory/test-results.md`.

## Urutan uji
1. **Load test:** import zip → driver muncul dengan nama `xamonxx naga8gen5 V0.x` tanpa error.
2. **Smoke test:** jalankan desktop Wine / game ringan (DX9/DX11 indie) 5 menit — tidak crash/black screen.
3. **GTA V Legacy:** masuk story mode, jalan-jalan kota 10 menit, setting sesuai `docs/INSTALL_AND_TUNING.md`.
4. **Pembanding:** ulangi langkah 3 dengan Turnip Gen8 V39 (setting identik).

## Kondisi wajib dicatat
Baterai %, charging/bypass on/off, Monster Mode on/off, suhu ruangan (kira-kira), casing on/off.

## Template laporan (salin ke `memory/test-results.md`)
```markdown
### YYYY-MM-DD — V0.x vs V39 — <game>
- Driver: xamonxx_naga8gen5V0.x (flavor perf/safe)
- Emulator: GameHub/Winlator versi …, DXVK …, Box64 preset …
- Setting game: resolusi …, preset …, FPS cap …
- Kondisi: baterai …%, bypass …, Monster Mode …, casing …
- Hasil V0.x: FPS rata-rata …, min …, crash? …, glitch? …, suhu (skin/CPU) …
- Hasil V39:  FPS rata-rata …, min …, crash? …, glitch? …, suhu …
- Kesimpulan: …
- Status: TESTED
```
