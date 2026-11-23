"""Audit AdrenoTools driver zips.

Usage: python -I tools/audit_driver.py <zip|folder> [...]
Checks: SHA256, CRC, file list (path traversal), meta.json, ELF aarch64,
Adreno 829 support string, Android HAL entry point (HMI), Mesa version.
"""
import hashlib
import os
import re
import struct
import sys
import zipfile


def audit(path):
    ok = True
    print("=" * 70)
    print(os.path.basename(path))
    print("SHA256", hashlib.sha256(open(path, "rb").read()).hexdigest())
    z = zipfile.ZipFile(path)
    bad_crc = z.testzip()
    print("CRC:", bad_crc or "OK")
    ok &= bad_crc is None
    for i in z.infolist():
        bad = i.filename.startswith("/") or ".." in i.filename
        ok &= not bad
        print(f"  {i.filename:45} {i.file_size:>10}" + ("  !!PATH" if bad else ""))
    for i in z.infolist():
        data = z.read(i)
        if i.filename.endswith(".json"):
            print("  meta:", data.decode(errors="replace").strip())
        elif data[:4] == b"\x7fELF":
            mach = struct.unpack("<H", data[18:20])[0]
            is64 = data[4] == 2
            a829 = b"Adreno (TM) 829" in data
            hmi = b"\x00HMI\x00" in data
            mesa = sorted({m.decode() for m in re.findall(rb"Mesa \d+\.\d+\.\d+[^\x00]{0,30}", data)})[:2]
            print(f"  ELF {i.filename}: 64-bit={is64} aarch64={mach == 183} Adreno829={a829} HMI={hmi}")
            print(f"   mesa: {mesa}")
            if i.filename == "libvulkan_freedreno.so":
                ok &= is64 and mach == 183 and a829 and hmi
        else:
            print(f"  WARNING non-ELF/non-json file: {i.filename}")
            ok = False
    print("RESULT:", "PASS" if ok else "FAIL")
    return ok


def main():
    files = []
    for arg in sys.argv[1:] or ["dist"]:
        if os.path.isdir(arg):
            files += [os.path.join(arg, f) for f in sorted(os.listdir(arg)) if f.endswith(".zip")]
        else:
            files.append(arg)
    results = [audit(f) for f in files]
    sys.exit(0 if results and all(results) else 1)


if __name__ == "__main__":
    main()
