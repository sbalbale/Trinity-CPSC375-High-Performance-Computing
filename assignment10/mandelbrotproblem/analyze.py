#!/usr/bin/env python3
"""
analyze.py — Speedup & Efficiency Analysis for MPI Mandelbrot Scaling Study
============================================================================

Reads timing_results.csv produced by run_study.sh, computes:
  - Median wall time per (study_type, size, nprocs)
  - Speedup  S(P) = T(1_worker) / T(P)      [relative to 2-proc baseline]
  - Efficiency E(P) = S(P) / (P - 1)        [(P-1) workers, rank 0 is master]
  - Amdahl's law fit to extract serial fraction f

Outputs:
  - Console table
  - strong_scaling.png  — speedup vs. P for each problem size
  - weak_scaling.png    — normalized time vs. P

Usage: python3 analyze.py [timing_results.csv]
"""

import sys
import csv
import statistics
from collections import defaultdict

try:
    import matplotlib.pyplot as plt
    import numpy as np
    HAS_MPL = True
except ImportError:
    HAS_MPL = False
    print("[warn] matplotlib/numpy not found — skipping plots. "
          "pip install matplotlib numpy to enable.")

# ── Load CSV ─────────────────────────────────────────────────────────────────
csvfile = sys.argv[1] if len(sys.argv) > 1 else "timing_results.csv"

# data[study_type][size_label][nprocs] = [t1, t2, ...]
data = defaultdict(lambda: defaultdict(lambda: defaultdict(list)))

with open(csvfile) as f:
    reader = csv.DictReader(f)
    for row in reader:
        study  = row["study_type"]
        label  = f"{row['width']}x{row['height']}"
        nprocs = int(row["nprocs"])
        t      = float(row["wall_time_s"])
        data[study][label][nprocs].append(t)

def median(vals):
    return statistics.median(vals)

# ── Strong Scaling ────────────────────────────────────────────────────────────
print("\n" + "═"*65)
print("  STRONG SCALING")
print("═"*65)
print(f"  {'Size':<12} {'P':>4}  {'T(P) s':>10}  {'Speedup':>8}  {'Efficiency':>10}")
print("─"*65)

ss_data = {}   # ss_data[label] = {nprocs: median_t}
for label, pdict in sorted(data["strong"].items()):
    sorted_p = sorted(pdict)
    t_base   = median(pdict[sorted_p[0]])  # smallest P is the baseline
    ss_data[label] = {}
    for p in sorted_p:
        t = median(pdict[p])
        speedup = t_base / t
        nworkers = p - 1  # rank 0 is master-only
        efficiency = speedup / nworkers if nworkers > 0 else float("nan")
        ss_data[label][p] = t
        print(f"  {label:<12} {p:>4}  {t:>10.4f}  {speedup:>8.3f}  {efficiency:>10.3f}")
    print()

# ── Weak Scaling ──────────────────────────────────────────────────────────────
print("═"*65)
print("  WEAK SCALING  (normalized time — ideal = 1.000)")
print("═"*65)
print(f"  {'P':>4}  {'Size':<12}  {'T(P) s':>10}  {'Norm T':>8}")
print("─"*65)

ws_norm = {}
p_list_ws = []
t_list_ws = []

ws_all = data["weak"]
first_entry = None

for label, pdict in sorted(ws_all.items()):
    for p, vals in sorted(pdict.items()):
        t = median(vals)
        if first_entry is None:
            first_entry = t
        norm = t / first_entry
        ws_norm[p] = norm
        p_list_ws.append(p)
        t_list_ws.append(t)
        print(f"  {p:>4}  {label:<12}  {t:>10.4f}  {norm:>8.3f}")

print()

# ── Amdahl's Law Fit ──────────────────────────────────────────────────────────
if HAS_MPL and ss_data:
    import numpy as np

    print("═"*65)
    print("  AMDAHL'S LAW FIT  S(P) = 1 / (f + (1-f)/P)")
    print("═"*65)

    for label, pdict in ss_data.items():
        procs   = sorted(pdict)
        t_base  = pdict[procs[0]]
        speedups = [t_base / pdict[p] for p in procs]
        p_arr   = np.array([float(p) for p in procs])
        s_arr   = np.array(speedups)

        # Least-squares fit: s ≈ 1/(f + (1-f)/p)  → linearize
        # 1/s = f + (1-f)/p  →  1/s = (1 - 1/p)*f + 1/p
        inv_s = 1.0 / s_arr
        X = 1.0 - 1.0 / p_arr
        f_fit = float(np.polyfit(X, inv_s - 1.0/p_arr, 0)[0])  # intercept only approx
        # Better: direct least squares
        A = np.column_stack([X, 1.0/p_arr])
        result = np.linalg.lstsq(A, inv_s, rcond=None)
        f_fit, _ = result[0]
        f_fit = max(0.0, min(1.0, f_fit))
        p_max = 1.0 / f_fit if f_fit > 0 else float("inf")
        print(f"  {label:<12}  serial fraction f ≈ {f_fit:.4f}  "
              f"→ theoretical max speedup ≈ {p_max:.1f}×")
    print()

# ── Plots ─────────────────────────────────────────────────────────────────────
if HAS_MPL:

    fig, axes = plt.subplots(1, 2, figsize=(12, 5))
    fig.suptitle("MPI Mandelbrot — Scaling Study", fontsize=14, fontweight="bold")

    # ── Strong scaling plot ──────────────────────────────────────────────────
    ax = axes[0]
    colors = plt.cm.tab10.colors

    for i, (label, pdict) in enumerate(sorted(ss_data.items())):
        procs    = sorted(pdict)
        t_base   = pdict[procs[0]]
        speedups = [t_base / pdict[p] for p in procs]
        ax.plot(procs, speedups, "o-", color=colors[i % 10], label=label, linewidth=2)

    # Ideal (linear) speedup line
    all_p = sorted({p for pd in ss_data.values() for p in pd})
    workers = [p - 1 for p in all_p]
    ax.plot(all_p, workers, "k--", linewidth=1, alpha=0.5, label="Ideal (P−1)")

    ax.set_xlabel("Number of MPI Processes (P)")
    ax.set_ylabel("Speedup S(P)")
    ax.set_title("Strong Scaling — Speedup")
    ax.legend()
    ax.grid(True, alpha=0.3)

    # ── Weak scaling plot ────────────────────────────────────────────────────
    ax2 = axes[1]
    ws_p = sorted(ws_norm)
    ws_t = [ws_norm[p] for p in ws_p]

    ax2.plot(ws_p, ws_t, "s-", color="steelblue", linewidth=2, label="Measured")
    ax2.axhline(1.0, color="k", linestyle="--", linewidth=1, alpha=0.5, label="Ideal")

    ax2.set_xlabel("Number of MPI Processes (P)")
    ax2.set_ylabel("Normalized Wall Time  T(P) / T(2)")
    ax2.set_title("Weak Scaling — Communication Overhead")
    ax2.legend()
    ax2.grid(True, alpha=0.3)

    plt.tight_layout()
    plt.savefig("scaling_results.png", dpi=150, bbox_inches="tight")
    print("Plot saved to: scaling_results.png")
    plt.show()
