#!/usr/bin/env python3
"""Summarise xamonxx naga8gen5 debug-driver CSV logs.

Usage:  python -I tools/analyze_log.py <log.csv> [more.csv ...] [--warmup 15]

The debug zip writes Download/xamonxx/log_<date>_<time>_<mode>_<pid>.csv with one row per
second. This prints a Markdown block ready to paste into memory/test-results.md.
Standard library only; the CSV is treated as untrusted data (parsed, never executed).
"""
import argparse
import csv
import math
import sys
from pathlib import Path


def num(v):
    try:
        f = float(v)
    except (TypeError, ValueError):
        return None
    return f if math.isfinite(f) else None


def col(rows, name):
    return [x for x in (num(r.get(name)) for r in rows) if x is not None]


def mean(xs):
    return sum(xs) / len(xs) if xs else None


def pct(xs, p):
    if not xs:
        return None
    s = sorted(xs)
    k = max(0, min(len(s) - 1, int(math.ceil(p / 100 * len(s))) - 1))
    return s[k]


def fmt(v, digits=1, unit=""):
    return "NA" if v is None else f"{v:.{digits}f}{unit}"


def load(path):
    meta, lines = {}, []
    with open(path, encoding="utf-8", errors="replace", newline="") as f:
        for line in f:
            if line.startswith("#"):
                for tok in line[1:].split():
                    if "=" in tok:
                        k, v = tok.split("=", 1)
                        meta.setdefault(k, v)
            elif line.strip():
                lines.append(line)
    rows = list(csv.DictReader(lines))
    return meta, rows


def summarise(path, warmup):
    meta, rows = load(path)
    if not rows:
        return f"### {Path(path).name}\n- Tidak ada baris data (game belum present frame / log kosong).\n"

    limit = num(meta.get("fps_limit")) or 0
    target = num(meta.get("target_fps")) or (limit if limit > 0 else 60.0)

    # Gameplay = from the first second the GPU is really working (loading screens run at
    # ~2-5 % GPU busy) or after --warmup, whichever is later; drop the last 3 s (game exit).
    t_game = next((num(r.get("t_s")) for r in rows if (num(r.get("gpu_busy_pct")) or 0) >= 30), None)
    t_start = max(warmup, t_game or 0)
    steady = [r for r in rows if (num(r.get("t_s")) or 0) >= t_start]
    steady = steady[:-3] if len(steady) > 6 else steady
    # Seconds with almost no presents = game in the background (e.g. owner switched to the
    # file manager to edit mode.txt); they are not gameplay.
    bg = sum(1 for r in steady if (num(r.get("fps")) or 0) < 5)
    steady = [r for r in steady if (num(r.get("fps")) or 0) >= 5] or steady or rows

    fps = col(steady, "fps")
    ft_max = col(steady, "frametime_max_ms")
    slow = sum(col(steady, "slow_frames"))
    minutes = len(steady) / 60
    ok_secs = sum(1 for x in fps if x >= 0.95 * target)

    out = [f"### {Path(path).name}"]
    out.append(f"- Mode: **{meta.get('mode', '?')}** (sumber {meta.get('source', '?')}), "
               f"fps limit {int(limit) if limit else 'off'}, GPU floor {meta.get('gpu_floor', '?')}, ADPF {meta.get('adpf', '?')}")
    last = rows[-1]
    out.append(f"- Status runtime: GPU floor KGSL **{last.get('gpu_floor_status', '?')}**, "
               f"ADPF **{last.get('adpf_status', '?')}**")
    out.append(f"- Durasi: {len(rows)} s total; gameplay dianalisis {len(steady)} s "
               f"(mulai t={t_start:.0f} s, loading/warm-up & 3 s terakhir dibuang"
               f"{f', {bg} s di background dibuang' if bg else ''})")
    out.append(f"- FPS: rata-rata **{fmt(mean(fps))}**, 1% terendah (per detik) {fmt(pct(fps, 1))}, "
               f"min {fmt(min(fps) if fps else None)}, max {fmt(max(fps) if fps else None)}")
    out.append(f"- Detik dengan fps ≥ 95% target ({target:.0f}): **{100 * ok_secs / len(fps):.1f}%**" if fps else "- Detik ≥ target: NA")
    out.append(f"- Frame time terburuk: {fmt(max(ft_max) if ft_max else None, 1, ' ms')}, p99 dari frame time maks per detik {fmt(pct(ft_max, 99), 1, ' ms')}; "
               f"frame lambat (>1.5× target): {int(slow)} ({fmt(slow / minutes if minutes else None, 1)}/menit)")
    if target <= 61:
        vs = sum(1 for x in ft_max if 30 <= x < 40)
        out.append(f"- Detik dengan frame 30–40 ms (tanda V-Sync miss / 1 frame tampil 2×): {vs} dari {len(ft_max)}")
    busy = col(steady, "gpu_busy_pct")
    out.append(f"- GPU: busy rata-rata {fmt(mean(busy), 0, '%')} (≥95% pada {sum(1 for x in busy if x >= 95)} dari {len(busy)} detik terbaca), "
               f"clock rata-rata {fmt(mean(col(steady, 'gpu_freq_mhz')), 0, ' MHz')}, "
               f"clock maks diizinkan {fmt(min(col(steady, 'gpu_max_mhz')) if col(steady, 'gpu_max_mhz') else None, 0, ' MHz')} (terendah)")
    c0, c6 = col(steady, "cpu0_mhz"), col(steady, "cpu6_mhz")
    out.append(f"- CPU clock: Performance (cpu0) rata {fmt(mean(c0), 0)} / maks {fmt(max(c0) if c0 else None, 0)} MHz (hardware 3322), "
               f"Prime (cpu6) rata {fmt(mean(c6), 0)} / maks {fmt(max(c6) if c6 else None, 0)} MHz (hardware 3802)")
    lim_now = col(steady, "fps_limit_now")
    if lim_now:
        out.append(f"- Thermal guard: batas fps terendah {min(lim_now):.0f}, langkah guard {int(max(col(steady, 'guard_steps') or [0]))}")
    for name, label in (("temp_skin_c", "Skin/shell"), ("temp_gpu_max_c", "GPU"),
                        ("temp_cpu_max_c", "CPU"), ("temp_batt_c", "Baterai")):
        t = col(rows, name)
        out.append(f"- Suhu {label}: awal {fmt(t[0] if t else None, 1, '°C')}, "
                   f"rata-rata {fmt(mean(t), 1, '°C')}, maks **{fmt(max(t) if t else None, 1, '°C')}**")

    # 5-minute buckets: does fps fall as temperature rises?
    out.append("")
    out.append("| Menit | FPS rata | FPS 1% | Skin maks °C | GPU maks °C | GPU MHz |")
    out.append("|---|---|---|---|---|---|")
    bucket = {}
    for r in rows:
        t = num(r.get("t_s")) or 0
        bucket.setdefault(int(t // 300), []).append(r)
    for b in sorted(bucket):
        rs = bucket[b]
        f_ = col(rs, "fps")
        sk = col(rs, "temp_skin_c")
        gp = col(rs, "temp_gpu_max_c")
        out.append(f"| {b * 5}–{b * 5 + 5} | {fmt(mean(f_))} | {fmt(pct(f_, 1))} | "
                   f"{fmt(max(sk) if sk else None)} | {fmt(max(gp) if gp else None)} | "
                   f"{fmt(mean(col(rs, 'gpu_freq_mhz')), 0)} |")
    return "\n".join(out) + "\n"


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("logs", nargs="+")
    ap.add_argument("--warmup", type=float, default=15.0, help="seconds to skip at start (default 15)")
    a = ap.parse_args()
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8")  # Windows consoles default to cp1252
    for p in a.logs:
        if not Path(p).is_file():
            sys.exit(f"not found: {p}")
        print(summarise(p, a.warmup))


if __name__ == "__main__":
    main()
