# GTA V command line profiles (launcher "Command line")

Launcher format: arguments separated by spaces or new lines; `# ...` comments out a WHOLE line
(no inline comments). Applied on next game start.

Parameter ranges come from the official Rockstar list "Full list of command line parameters for Grand Theft Auto V on PC"
(support.rockstargames.com; the page timed out on 2026-11-24) as mirrored by citadelservers.com (re-checked 2026-11-24):
`-shaderQuality 0-2`, `-shadowQuality 0-3`, `-reflectionQuality 0-3`, `-SSAO 0-2`, `-postFX 0-3`,
`-grassQuality 0-5`, `-particleQuality 0-2`, `-textureQuality 0-2`, `-waterQuality 0-1`, `-tessellation 0-3`,
`-fxaa 0-3`, `-anisotropicQualityLevel 0-16`, `-lodScale / -pedLodBias / -vehicleLodBias / -cityDensity 0.0-1.0`,
`-noInGameDOF` (flag), `-fogVolumes` / `-particleShadows` (enable flags), `-frameLimit` (number of vsyncs — do not use).

## Which profile for which driver (V0.5)
| Driver zip | Profile | Resolution (start) | Launcher Target FPS | V-Sync |
|---|---|---|---|---|
| `V0.5-cool60` | C — Cool60 sharp | 1568×718 | 60 (required; the driver only has a 63 fps safety cap) | ON (required) |
| `V0.5-monster90` | A — 90 fps | 1310×600 | **Unlimited** (the driver paces at 90; a game lock at 90 + the driver = double pacing) | OFF |
| `V0.5-monster120` | B — 120 fps | 1280×586 → 1176×539 if it stays below 100 | **Unlimited** (the driver caps at 120) | OFF |

Resolutions on the panel's own ratio (2750×1260 = 2.18:1), so nothing is stretched:
1568×718 (57%) · 1440×660 (52%) · 1310×600 (48%) · 1280×586 (47%) · 1176×539 (43%).
V0.4 logs (no command line, 1556×718 / 1310×600, mapping not recorded): Cool60 locked 60 = 59.2 fps avg with GPU 79% busy;
Monster90 held 90 for ±20 s with GPU 73–95% busy; Monster120 best run 95.9 fps avg with GPU 98% busy (GPU-bound).

"Not blurry" in practice: keep the resolution as high as the fps target allows, turn **in-game DOF off**
(`-noInGameDOF`), keep **FXAA off** (FXAA softens the whole image), and raise **anisotropic filtering**
(sharper textures at an angle, cheap on the GPU). The driver itself never lowers image quality.

## Profile C: Cool60 sharp — steady 60, low heat
Basis: V0.2 benchmark with similar settings (shadow 1, reflection 1, texture 1, postFX 1, grass 1, AF 0) averaged 77.8 fps
at 1568×718 → about 25% headroom above 60 (test-results 2026-11-23). This profile spends a little of it on sharpness.
```
# xamonxx profile C - Cool60 sharp
-shaderQuality 1
-shadowQuality 1
-reflectionQuality 1
-waterQuality 0
-SSAO 0
-postFX 1
-tessellation 0
-grassQuality 1
-particleQuality 1
-textureQuality 1
-anisotropicQualityLevel 16
-fxaa 0
-noInGameDOF
-lodScale 0.7
-cityDensity 0.5
-pedLodBias 0.3
-vehicleLodBias 0.3
```
If the debug log shows ≥95% of seconds at 60 fps **and** GPU busy below ±80%, try a higher resolution
(e.g. 1792×821). If the edges look too jagged, use `-fxaa 1` (slightly softer). NOT TESTED.

## Profile A: target 90 fps (balanced) — for Monster90
```
# xamonxx profile A - target 90 fps
-shaderQuality 0
-shadowQuality 1
-reflectionQuality 0
-waterQuality 0
-SSAO 0
-postFX 0
-tessellation 0
-grassQuality 0
-particleQuality 0
-textureQuality 1
-anisotropicQualityLevel 4
-fxaa 0
-noInGameDOF
-lodScale 0.6
-cityDensity 0.4
-pedLodBias 0.2
-vehicleLodBias 0.2
```

## Profile B: target 120 fps (competitive) — for Monster120
```
# xamonxx profile B - target 120 fps
-shaderQuality 0
-shadowQuality 0
-reflectionQuality 0
-waterQuality 0
-SSAO 0
-postFX 0
-tessellation 0
-grassQuality 0
-particleQuality 0
-textureQuality 0
-anisotropicQualityLevel 2
-fxaa 0
-noInGameDOF
-lodScale 0.4
-cityDensity 0.25
-pedLodBias 0.0
-vehicleLodBias 0.1
```
Profile B also needs a lower resolution (for example ±1280×586) for a chance at 120.

## Notes
- `-fogVolumes 0` / `-particleShadows 0`: these are enable flags; whether `0` disables them is UNVERIFIED.
  They are left out above (off unless enabled).
- Do not use `-frameLimit`, `-HDStreamingInFlight`, `-shadowLongShadows`, `-hdr`, `-txaa`, or `-multiSample` above 1.
- Change one thing at a time and re-run the benchmark or a debug-driver session (`docs/TESTING.md`).
