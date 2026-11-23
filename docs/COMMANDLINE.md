# GTA V command line profiles (launcher "Command line")

Launcher format: arguments separated by spaces or new lines; `# ...` comments out a WHOLE line
(no inline comments). Applied on next game start.

Parameter ranges come from the official Rockstar list (mirrored by citadelservers.com):
`-shaderQuality 0-2`, `-shadowQuality 0-3`, `-reflectionQuality 0-3`, `-SSAO 0-2`, `-postFX 0-3`,
`-grassQuality 0-5`, `-particleQuality 0-2`, `-textureQuality 0-2`, `-waterQuality 0-1`, `-tessellation 0-3`,
`-fxaa 0-3`, `-anisotropicQualityLevel 0-16`, `-lodScale / -pedLodBias / -vehicleLodBias / -cityDensity 0.0-1.0`,
`-noInGameDOF` (flag), `-fogVolumes` / `-particleShadows` (enable flags), `-frameLimit` (number of vsyncs — do not use).

## Profile A: target 90 fps (balanced) — start here
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
-anisotropicQualityLevel 2
-fxaa 1
-noInGameDOF
-lodScale 0.6
-cityDensity 0.4
-pedLodBias 0.2
-vehicleLodBias 0.2
```

## Profile B: target 120 fps (competitive)
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
-anisotropicQualityLevel 0
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
- Change one thing at a time and re-run the benchmark (`docs/TESTING.md`).
