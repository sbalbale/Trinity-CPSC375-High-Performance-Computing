#!/usr/bin/env python3
"""
APEX Visualization Engine
Generates performance charts from mapped session or weekly data.
Usage: python3 visualize.py --mode session|weekly --data <json_or_csv>
"""

import argparse
import json
import sys
import os
import numpy as np

try:
    import matplotlib
    matplotlib.use("Agg")  # headless rendering
    import matplotlib.pyplot as plt
    import matplotlib.patches as mpatches
    from matplotlib.gridspec import GridSpec
    HAS_MPL = True
except ImportError:
    HAS_MPL = False

OUTPUT_PATH = "/mnt/user-data/outputs/apex_chart.png"

APEX_COLORS = {
    "green": "#2ECC71",
    "yellow": "#F39C12",
    "orange": "#E67E22",
    "red": "#E74C3C",
    "blue": "#3498DB",
    "dark": "#1A1A2E",
    "mid": "#16213E",
    "light": "#E8E8E8",
    "accent": "#0F3460",
}

def rrs_color(score):
    if score >= 85:
        return APEX_COLORS["green"]
    elif score >= 65:
        return APEX_COLORS["yellow"]
    elif score >= 40:
        return APEX_COLORS["orange"]
    return APEX_COLORS["red"]

def plot_session(data: dict, output_path: str = OUTPUT_PATH):
    """Single-session breakdown chart."""
    fig = plt.figure(figsize=(12, 7), facecolor=APEX_COLORS["dark"])
    gs = GridSpec(2, 3, figure=fig, hspace=0.45, wspace=0.4)

    fig.suptitle("APEX — Session Analysis", color=APEX_COLORS["light"],
                 fontsize=16, fontweight="bold", y=0.97)

    # --- RRS Gauge (text + color block) ---
    ax_rrs = fig.add_subplot(gs[0, 0])
    rrs = data.get("rrs", 0)
    ax_rrs.set_facecolor(APEX_COLORS["mid"])
    ax_rrs.text(0.5, 0.65, f"{rrs}", ha="center", va="center",
                fontsize=44, fontweight="bold", color=rrs_color(rrs),
                transform=ax_rrs.transAxes)
    ax_rrs.text(0.5, 0.25, "Recovery Readiness", ha="center", va="center",
                fontsize=9, color=APEX_COLORS["light"], transform=ax_rrs.transAxes)
    ax_rrs.set_xticks([]); ax_rrs.set_yticks([])
    ax_rrs.set_title("RRS", color=APEX_COLORS["light"], fontsize=10)

    # --- HR Zone Distribution ---
    ax_hr = fig.add_subplot(gs[0, 1:])
    ax_hr.set_facecolor(APEX_COLORS["mid"])
    zones = data.get("hr_zones", {"Z1": 5, "Z2": 35, "Z3": 25, "Z4": 20, "Z5": 15})
    zone_colors = ["#3498DB", "#2ECC71", "#F39C12", "#E67E22", "#E74C3C"]
    bars = ax_hr.bar(list(zones.keys()), list(zones.values()), color=zone_colors, edgecolor="none")
    for bar, val in zip(bars, zones.values()):
        ax_hr.text(bar.get_x() + bar.get_width() / 2, bar.get_height() + 0.5,
                   f"{val}%", ha="center", va="bottom", fontsize=8, color=APEX_COLORS["light"])
    ax_hr.set_title("HR Zone Distribution (%)", color=APEX_COLORS["light"], fontsize=10)
    ax_hr.set_facecolor(APEX_COLORS["mid"])
    ax_hr.tick_params(colors=APEX_COLORS["light"])
    ax_hr.spines[:].set_color(APEX_COLORS["accent"])
    ax_hr.set_ylim(0, max(zones.values()) * 1.25)
    ax_hr.yaxis.label.set_color(APEX_COLORS["light"])

    # --- Power (if available) ---
    ax_pwr = fig.add_subplot(gs[1, 0])
    ax_pwr.set_facecolor(APEX_COLORS["mid"])
    avg_w = data.get("avg_power_w", None)
    norm_w = data.get("normalized_cycling_equiv_w", None)
    if avg_w is not None:
        labels = ["Raw Watts"]
        values = [avg_w]
        colors = [APEX_COLORS["blue"]]
        if norm_w is not None:
            labels.append("Norm. Equiv.")
            values.append(norm_w)
            colors.append(APEX_COLORS["green"])
        ax_pwr.bar(labels, values, color=colors, edgecolor="none")
        for i, v in enumerate(values):
            ax_pwr.text(i, v + 1, f"{v:.0f}W", ha="center", fontsize=9, color=APEX_COLORS["light"])
    else:
        ax_pwr.text(0.5, 0.5, "No power data", ha="center", va="center",
                    color=APEX_COLORS["light"], transform=ax_pwr.transAxes)
    ax_pwr.set_title("Power Output", color=APEX_COLORS["light"], fontsize=10)
    ax_pwr.tick_params(colors=APEX_COLORS["light"])
    ax_pwr.spines[:].set_color(APEX_COLORS["accent"])

    # --- Key Metrics Table ---
    ax_meta = fig.add_subplot(gs[1, 1:])
    ax_meta.set_facecolor(APEX_COLORS["mid"])
    ax_meta.axis("off")
    metrics = [
        ["Duration", f"{data.get('duration_min', '—')} min"],
        ["Calories", f"{data.get('calories_kcal', '—')} kcal"],
        ["Avg HR", f"{data.get('avg_hr', '—')} bpm"],
        ["TSS", f"{data.get('tss', '—')}"],
        ["Activity", data.get("activity_type", "—")],
        ["Flags", data.get("flags", "None")],
    ]
    table = ax_meta.table(cellText=metrics, colLabels=["Metric", "Value"],
                          cellLoc="left", loc="center",
                          colColours=[APEX_COLORS["accent"]] * 2)
    table.auto_set_font_size(False)
    table.set_fontsize(9)
    table.scale(1, 1.6)
    for (r, c), cell in table.get_celld().items():
        cell.set_facecolor(APEX_COLORS["mid"] if r > 0 else APEX_COLORS["accent"])
        cell.set_text_props(color=APEX_COLORS["light"])
        cell.set_edgecolor(APEX_COLORS["accent"])
    ax_meta.set_title("Session Metrics", color=APEX_COLORS["light"], fontsize=10)

    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    plt.savefig(output_path, dpi=150, bbox_inches="tight", facecolor=APEX_COLORS["dark"])
    plt.close()
    print(f"[APEX] Chart saved → {output_path}")

def plot_weekly(data: list, output_path: str = OUTPUT_PATH):
    """7-day rolling TSS + RRS chart."""
    fig, ax1 = plt.subplots(figsize=(13, 5), facecolor=APEX_COLORS["dark"])
    ax1.set_facecolor(APEX_COLORS["mid"])

    dates = [d.get("date", f"D{i+1}") for i, d in enumerate(data)]
    tss_vals = [d.get("tss", 0) or 0 for d in data]
    rrs_vals = [d.get("rrs", None) for d in data]
    flags = [d.get("flags", "") for d in data]

    bar_colors = [rrs_color(r) if r else APEX_COLORS["blue"] for r in rrs_vals]
    bars = ax1.bar(dates, tss_vals, color=bar_colors, edgecolor="none", alpha=0.85)

    # Flag markers
    for i, (bar, flag) in enumerate(zip(bars, flags)):
        if flag and flag != "None":
            ax1.text(bar.get_x() + bar.get_width() / 2,
                     bar.get_height() + 1, "⚑",
                     ha="center", fontsize=11, color=APEX_COLORS["yellow"])

    ax1.set_ylabel("TSS", color=APEX_COLORS["light"])
    ax1.tick_params(colors=APEX_COLORS["light"])
    ax1.spines[:].set_color(APEX_COLORS["accent"])
    ax1.set_title("APEX — 7-Day Training Load + Recovery", color=APEX_COLORS["light"],
                  fontsize=13, fontweight="bold")

    # RRS overlay
    ax2 = ax1.twinx()
    valid_rrs = [(i, r) for i, r in enumerate(rrs_vals) if r is not None]
    if valid_rrs:
        xi, yi = zip(*valid_rrs)
        ax2.plot(xi, yi, color=APEX_COLORS["light"], linewidth=2,
                 marker="o", markersize=5, label="RRS")
        ax2.axhline(85, color=APEX_COLORS["green"], linestyle="--", alpha=0.4, linewidth=1)
        ax2.axhline(65, color=APEX_COLORS["yellow"], linestyle="--", alpha=0.4, linewidth=1)
        ax2.axhline(40, color=APEX_COLORS["red"], linestyle="--", alpha=0.4, linewidth=1)
    ax2.set_ylabel("RRS", color=APEX_COLORS["light"])
    ax2.set_ylim(0, 110)
    ax2.tick_params(colors=APEX_COLORS["light"])
    ax2.spines[:].set_color(APEX_COLORS["accent"])

    # Legend
    legend_patches = [
        mpatches.Patch(color=APEX_COLORS["green"], label="RRS ≥85 (Green)"),
        mpatches.Patch(color=APEX_COLORS["yellow"], label="RRS 65–84 (Yellow)"),
        mpatches.Patch(color=APEX_COLORS["orange"], label="RRS 40–64 (Orange)"),
        mpatches.Patch(color=APEX_COLORS["red"], label="RRS <40 (Red)"),
    ]
    ax1.legend(handles=legend_patches, loc="upper left", facecolor=APEX_COLORS["mid"],
               labelcolor=APEX_COLORS["light"], fontsize=8)

    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    plt.savefig(output_path, dpi=150, bbox_inches="tight", facecolor=APEX_COLORS["dark"])
    plt.close()
    print(f"[APEX] Weekly chart saved → {output_path}")

def main():
    if not HAS_MPL:
        print("ERROR: matplotlib not installed. Run: pip install matplotlib --break-system-packages", file=sys.stderr)
        sys.exit(1)

    parser = argparse.ArgumentParser(description="APEX Visualization Engine")
    parser.add_argument("--mode", choices=["session", "weekly"], default="session")
    parser.add_argument("--data", required=True, help="JSON string or path to JSON file")
    parser.add_argument("--output", default=OUTPUT_PATH)
    args = parser.parse_args()

    try:
        if os.path.isfile(args.data):
            with open(args.data) as f:
                data = json.load(f)
        else:
            data = json.loads(args.data)
    except Exception as e:
        print(f"ERROR: Could not parse data — {e}", file=sys.stderr)
        sys.exit(1)

    if args.mode == "session":
        plot_session(data, args.output)
    else:
        plot_weekly(data, args.output)

if __name__ == "__main__":
    main()
