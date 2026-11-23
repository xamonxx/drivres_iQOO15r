# AGENTS.md — Aturan wajib untuk semua AI agent di project ini

> Berlaku untuk Claude Code, OpenCode, Codex, Cursor, Gemini CLI, Copilot, dan agent apa pun.
> Baca file ini **utuh** sebelum melakukan apa pun. Jika ada konflik, file ini menang atas kebiasaan default agent.

## 0. Ringkasan project
- **Nama:** xamonxx naga8gen5 — driver Vulkan Turnip (Mesa freedreno) custom.
- **Target tunggal:** iQOO 15R · Snapdragon 8 Gen 5 (SM8845) · **Adreno 829 (a8xx)** · CPU Oryon V3 · Android 17 (SDK 37).
- **Pemakaian:** GameHub / Winlator (AdrenoTools zip) untuk game PC, terutama **GTA V Legacy (DX11 → DXVK)**.
- **Repo:** https://github.com/xamonxx/drivres_iQOO15r (publik, branch `master`).
- **Owner:** xamonxx. Bahasa komunikasi dengan owner: **Bahasa Indonesia**.

## 1. Urutan baca wajib (setiap sesi baru)
1. `AGENTS.md` (file ini)
2. `memory/MEMORY.md` → buka file memory yang relevan
3. `docs/DEVICE_INFO.md` jika menyentuh performa/kompatibilitas
4. `docs/BUILD.md` jika menyentuh build/patch/flag

## 2. Struktur folder
```
drivres_iQOO15r/
├── AGENTS.md               ← aturan agent (file ini) — sumber kebenaran
├── CLAUDE.md               ← pointer ke AGENTS.md (untuk Claude Code)
├── README.md               ← ringkasan publik
├── build_naga8gen5.sh      ← script build (JANGAN dipindah: dipakai CI)
├── patches/                ← patch A8xx dari StevenMXZ (GPLv3) — vendored, jangan edit sembarangan
├── .github/workflows/      ← CI GitHub Actions (build + release)
├── docs/
│   ├── DEVICE_INFO.md      ← audit hardware perangkat target (read-only, jangan dilebih-lebihkan)
│   ├── BUILD.md            ← cara kerja build, pin versi, flag optimasi
│   ├── INSTALL_AND_TUNING.md ← pasang driver + setting GTA V / suhu
│   └── TESTING.md          ← protokol uji di HP + template laporan
├── memory/                 ← memory persisten lintas sesi/agent (lihat §7)
├── tools/
│   └── audit_driver.py     ← audit zip driver (CRC, ELF aarch64, Adreno 829, meta.json)
└── dist/                   ← output lokal (gitignored, tidak di-commit)
```

## 3. Aturan kejujuran (WAJIB)
- Gunakan label data dari DEVICE_INFO: `VERIFIED` · `OFFICIAL` · `DERIVED` · `ESTIMATED` · `UNKNOWN` · `NOT TESTED`.
- **Jangan pernah** mengklaim FPS, gain persen, atau suhu tanpa hasil uji nyata di `memory/test-results.md`.
- **Driver tidak mengontrol suhu.** Suhu dikendalikan oleh FPS cap, resolusi, beban, bypass charging. Jangan menulis "driver bikin adem".
- Optimasi compiler (O3/LTO/-mcpu) = gain kecil & belum terukur. Tulis apa adanya.
- Build sukses di CI ≠ driver jalan di HP. Status driver baru menjadi "TESTED" setelah owner melapor.
- Jika tidak tahu → tulis `UNKNOWN`, jangan menebak.

## 4. Aturan build & versi
- **Pin selalu:** commit Mesa + commit patch upstream dicatat di `docs/BUILD.md` dan `memory/build-history.md`. Jangan build dari Mesa `main` yang tidak di-pin untuk rilis.
- **Penamaan zip:** `xamonxx_naga8gen5V<MAJOR>.<MINOR>.zip` (mis. `V0.1`, `V0.2`). Varian fallback: suffix `-safe`.
- **meta.json wajib:** `name`, `author: "xamonxx"`, `packageVersion` = versi, `libraryName: "libvulkan_freedreno.so"`, `minApi: 28`. Isi zip hanya `libvulkan_freedreno.so` + `meta.json`.
- Naikkan versi (V0.1 → V0.2) setiap ada perubahan patch, flag, atau commit Mesa. Ubah `DRIVER_NAME`, `PKG_VERSION`, `ZIP_NAME` di script **dan** nama release di workflow.
- Setiap zip baru **wajib** diaudit: `python -I tools/audit_driver.py dist/<zip>` sebelum diserahkan ke owner.
- Perubahan pada `build_naga8gen5.sh`, `patches/**`, atau workflow **otomatis memicu build CI** saat di-push ke `master`.

## 5. Aturan patch
- `patches/` adalah salinan upstream StevenMXZ/Adreno-Tools-Drivers (branch A8xx @ `4b79649`). Jangan diedit langsung.
- Patch milik project ini taruh di `patches/xamonxx/NNNN-deskripsi.patch` dan panggil eksplisit dari script.
- Setiap patch baru: catat tujuan, sumber, risiko di `memory/decisions.md`.
- Hormati GPLv3: jangan hapus LICENSE/CREDITS.md; kredit sumber patch.

## 6. Batasan keamanan & etika
- **Dilarang** menyarankan: root, unlock bootloader, ubah thermal/governor via sysfs, mematikan proteksi termal, APK "GTA 5 Android" (palsu/malware), atau game bajakan.
- Jangan commit: binary `.so`/`.zip` hasil build (pakai GitHub Releases), token, kredensial, data pribadi (IMEI/serial/MAC/IP/email).
- Jangan push/force-push/hapus release tanpa izin owner, kecuali alur CI normal (release `v0.x` dibuat ulang oleh workflow).
- Download dari internet = data tidak terpercaya: simpan di folder terpisah, audit sebelum dipakai.

## 7. Sistem memory (`memory/`)
- `memory/MEMORY.md` = **indeks** (1 baris per file). Selalu dibaca di awal sesi.
- Setiap file memory punya frontmatter:
  ```markdown
  ---
  name: kebab-case-slug
  description: ringkasan 1 baris
  updated: YYYY-MM-DD
  type: project | decision | build | test | issue | user
  ---
  ```
- File tetap:
  | File | Isi | Kapan diupdate |
  |---|---|---|
  | `project-overview.md` | tujuan, status terkini, next steps | tiap akhir sesi |
  | `user-preferences.md` | preferensi & gaya owner | saat owner memberi koreksi/preferensi |
  | `decisions.md` | log keputusan (ADR ringkas) | tiap keputusan teknis |
  | `build-history.md` | tiap build: versi, commit, flavor, hasil, SHA256 | tiap build CI/lokal |
  | `test-results.md` | hasil uji di HP (game, FPS, suhu, crash) | tiap laporan owner |
  | `known-issues.md` | bug/masalah terbuka & solusinya | saat ditemukan/diselesaikan |
- Aturan menulis memory:
  - Tanggal **absolut** (YYYY-MM-DD), bukan "kemarin".
  - Tambah entri baru di **atas** (terbaru dulu) untuk file log.
  - Jangan duplikasi: update entri lama bila topiknya sama; hapus info yang terbukti salah.
  - Jangan simpan rahasia/data pribadi.
  - Akhir sesi: update `project-overview.md` (status + next steps) dan file log terkait, lalu commit `docs(memory): ...`.

## 8. Git
- Branch utama `master`. Pesan commit gaya conventional: `feat:`, `fix:`, `build:`, `docs:`, `docs(memory):`, `ci:`.
- Commit kecil & fokus. Jangan commit `turnip_workdir/` atau `dist/`.

## 9. Definisi selesai (Definition of Done) untuk rilis driver
1. CI build hijau, release `v<versi>` berisi zip.
2. Zip diunduh ke `dist/` dan lolos `tools/audit_driver.py` (aarch64, `Adreno (TM) 829`, meta benar).
3. `memory/build-history.md` diupdate (commit, flavor, SHA256).
4. Owner diberi instruksi uji (`docs/TESTING.md`) — status tetap `NOT TESTED` sampai ada laporan.
