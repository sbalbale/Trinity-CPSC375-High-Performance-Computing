#!/usr/bin/env python3
"""
analyze.py — Speedup & Efficiency Analysis for MPI N-body Scaling Study
========================================================================

Reads timing_results.csv produced by run_study.sh and computes:
  - Median wall time per (study_type, N, nprocs)
  - Speedup  S(P) = T(1) / T(P)          [baseline: single process]
  - Efficiency  E(P) = S(P) / P           [all P ranks compute]
  - Amdahl's law fit to extract serial fraction f
  - Weak scaling normalized time and theoretical O(P) growth overlay

Note on weak scaling: unlike embarrassingly parallel problems, the all-pairs
N-body algorithm is O(N²). Holding N/P = const means total work grows as
O(N² / P) = O(P · base²), so each process's work scales as O(N) = O(P·base).
Ideal weak scaling (constant time) is theoretically unachievable; the plot
shows the expected linear-in-P growth and quantifies actual overhead.

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

# ── Load CSV ──────────────────────────────────────────────────────────────────
csvfile = sys.argv[1] if len(sys.argv) > 1 else "timing_results.csv"

# data[study_type][N_label][nprocs] = [t1, t2, ...]
data = defaultdict(lambda: defaultdict(lambda: defaultdict(list)))

with open(csvfile) as f:
    reader = csv.DictReader(f)
    for row in reader:
        study  = row["study_type"]
        label  = row["N"]                   # particle count as string label
        nprocs = int(row["nprocs"])
        t      = float(row["wall_time_s"])
        data[study][label][nprocs].append(t)

def median(vals):
    return statistics.median(vals)

# ── Strong Scaling ────────────────────────────────────────────────────────────
print("\n" + "═"*70)
print("  STRONG SCALING")
print("═"*70)
print(f"  {'N':>8} {'P':>4}  {'T(P) s':>10}  {'Speedup':>8}  {'Efficiency':>10}")
print("─"*70)

ss_data = {}   # ss_data[label] = {nprocs: median_t}

for label, pdict in sorted(data["strong"].items(), key=lambda x: int(x[0])):
    sorted_p = sorted(pdict)
    # Baseline: P=1 (single process, all ranks compute)
    t_base   = median(pdict[1]) if 1 in pdict else median(pdict[sorted_p[0]])
    ss_data[label] = {}

    for p in sorted_p:
        t = median(pdict[p])
        speedup    = t_base / t
        efficiency = speedup / p   # all P ranks compute (no idle master)
        ss_data[label][p] = t
        print(f"  {label:>8} {p:>4}  {t:>10.4f}  {speedup:>8.3f}  {efficiency:>10.3f}")
    print()

# ── Weak Scaling ──────────────────────────────────────────────────────────────
print("═"*70)
print("  WEAK SCALING  (each process owns a fixed number of particles)")
print("  Note: O(N²) algorithm → time grows as O(P) even at ideal scaling")
print("═"*70)
print(f"  {'P':>4}  {'N':>8}  {'T(P) s':>10}  {'Norm T':>8}  {'Ideal O(P)':>12}")
print("─"*70)

ws_norm = {}
ws_raw  = {}

ws_all = data["weak"]
# Baseline: smallest P (should be P=1)
all_p_ws = sorted({p for pd in ws_all.values() for p in pd})
p_base_ws = all_p_ws[0]

# Find baseline time: P=p_base_ws with the corresponding N
# For weak scaling: N = WEAK_BASE * P; find T(p_base_ws)
t_base_ws = None
for label, pdict in ws_all.items():
    if p_base_ws in pdict:
        t_base_ws = median(pdict[p_base_ws])
        break

for label, pdict in sorted(ws_all.items(), key=lambda x: int(x[0])):
    for p, vals in sorted(pdict.items()):
        t    = median(vals)
        norm = t / t_base_ws if t_base_ws else float("nan")
        ideal_linear = float(p) / p_base_ws   # O(P) prediction
        ws_norm[p]  = norm
        ws_raw[p]   = t
        print(f"  {p:>4}  {label:>8}  {t:>10.4f}  {norm:>8.3f}  {ideal_linear:>12.3f}")

print()

# ── Amdahl's Law Fit ──────────────────────────────────────────────────────────
if HAS_MPL and ss_data:
    import numpy as np

    print("═"*70)
    print("  AMDAHL'S LAW FIT  S(P) = 1 / (f + (1−f)/P)")
    print("═"*70)

    for label, pdict in ss_data.items():
        procs    = sorted(pdict)
        t_b      = pdict[procs[0]]
        speedups = [t_b / pdict[p] for p in procs]
        p_arr    = np.array([float(p) for p in procs])
        s_arr    = np.array(speedups)

        # Linearized least-squares:  1/S = f*(1 − 1/P) + 1/P
        inv_s  = 1.0 / s_arr
        X      = 1.0 - 1.0 / p_arr
        A      = np.column_stack([X, 1.0 / p_arr])
        result = np.linalg.lstsq(A, inv_s, rcond=None)
        f_fit  = float(result[0][0])
        f_fit  = max(0.0, min(1.0, f_fit))
        p_max  = 1.0 / f_fit if f_fit > 0 else float("inf")
        print(f"  N={label:<8}  serial fraction f ≈ {f_fit:.4f}  "
              f"→ theoretical max speedup ≈ {p_max:.1f}×")
    print()

# ── Plots ──────────────────────────────────────────────────────────────────────
if HAS_MPL:
    fig, axes = plt.subplots(1, 2, figsize=(13, 5))
    fig.suptitle("MPI N-body — Scaling Study (All-Pairs, Leapfrog)",
                 fontsize=14, fontweight="bold")

    # ── Strong scaling ────────────────────────────────────────────────────
    ax = axes[0]
    colors = plt.cm.tab10.colors

    for i, (label, pdict) in enumerate(sorted(ss_data.items(),
                                               key=lambda x: int(x[0]))):
        procs    = sorted(pdict)
        t_b      = pdict[procs[0]]
        speedups = [t_b / pdict[p] for p in procs]
        ax.plot(procs, speedups, "o-", color=colors[i % 10],
                label=f"N={label}", linewidth=2, markersize=5)

    # Ideal (linear) speedup
    all_p = sorted({p for pd in ss_data.values() for p in pd})
    ax.plot(all_p, all_p, "k--", linewidth=1.2, alpha=0.5, label="Ideal (P)")

    ax.set_xlabel("Number of MPI Processes (P)")
    ax.set_ylabel("Speedup S(P) = T(1) / T(P)")
    ax.set_title("Strong Scaling — Speedup")
    ax.legend()
    ax.grid(True, alpha=0.3)

    # ── Weak scaling ──────────────────────────────────────────────────────
    ax2 = axes[1]
    ws_p = sorted(ws_norm)
    ws_t = [ws_norm[p] for p in ws_p]

    ax2.plot(ws_p, ws_t, "s-", color="steelblue", linewidth=2,
             markersize=5, label="Measured T(P)/T(1)")

    # Theoretical O(P) growth line
    p_base = ws_p[0]
    ideal_p = [float(p) / p_base for p in ws_p]
    ax2.plot(ws_p, ideal_p, "r--", linewidth=1.2, alpha=0.7,
             label="Expected O(P) growth")
    ax2.axhline(1.0, color="k", linestyle=":", linewidth=1.0, alpha=0.4,
                label="Ideal (constant time)")

    ax2.set_xlabel("Number of MPI Processes (P)")
    ax2.set_ylabel("Normalized Wall Time  T(P) / T(1)")
    ax2.set_title("Weak Scaling — O(N²) Complexity Overhead")
    ax2.legend()
    ax2.grid(True, alpha=0.3)

    plt.tight_layout()
    plt.savefig("scaling_results.png", dpi=150, bbox_inches="tight")
    print("Plot saved to: scaling_results.png")
    plt.show()
