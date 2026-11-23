# drivres_iQOO15r

**xamonxx naga8gen5 V0.1**: a Turnip (Mesa freedreno) Vulkan driver for the **Snapdragon 8 Gen 5 / Adreno 829** (iQOO 15R), packaged as an AdrenoTools zip for GameHub and Winlator.

## What it contains
- The A8xx patch set from [StevenMXZ/Adreno-Tools-Drivers](https://github.com/StevenMXZ/Adreno-Tools-Drivers) (A8xx branch @ `4b79649`), pinned to Mesa `c5ef4bdf`.
- Performance build flags: `-O3`, ThinLTO, `-mcpu=oryon-1` tuning, `NDEBUG` (assertions off), and lld `--gc-sections` / `--icf=safe`.
- If the perf build fails, CI falls back to a "safe" build that uses the upstream release flags.

## Project layout
| Path | Isi |
|---|---|
| `AGENTS.md` / `CLAUDE.md` | Aturan wajib untuk AI agent |
| `memory/` | Memory persisten lintas sesi. Mulai dari `memory/MEMORY.md` |
| `docs/` | `DEVICE_INFO.md` (audit HP), `BUILD.md`, `INSTALL_AND_TUNING.md`, `TESTING.md` |
| `build_naga8gen5.sh`, `patches/`, `.github/workflows/` | Build driver (CI) |
| `tools/audit_driver.py` | Audit zip driver |
| `dist/` | Output lokal (tidak di-commit) |

## Build
GitHub Actions: **Actions → Build xamonxx naga8gen5 driver → Run workflow**. The zip is published under **Releases → v0.1**.

Local build (Linux x86_64): `bash build_naga8gen5.sh`. Use `FLAVOR=safe` for the fallback build.

## Install
GameHub: open the GPU driver settings and import `xamonxx_naga8gen5V0.1.zip`.
Winlator: go to Container → Graphics Driver → Install, then pick the zip. Do not extract the zip first.

## Notes
- Not tested on a device yet. If a game crashes, go back to Turnip Gen8 V39.
- A driver does not control device temperature. To keep the phone cool, cap FPS at 30–40, play at 720p and use bypass charging.

## Credits / License
GPLv3 (see LICENSE). Credits go to the Mesa/freedreno contributors, StevenMXZ, whitebelyash, WinNative-Emu, Droid-Deck, Vauzi-17 and The412Banner. See the upstream CREDITS.md for details.
