#!/usr/bin/env python3
"""
APEX CSV Schema Mapper
Dynamically maps Strava, Whoop, or Garmin CSV exports to a unified schema.
Usage: python3 csv_mapper.py --file <path_to_csv>
"""

import argparse
import pandas as pd
import json
import sys

# Canonical output schema
CANONICAL_COLUMNS = [
    "date", "activity_type", "duration_min", "distance_km",
    "avg_hr", "max_hr", "avg_power_w", "max_power_w",
    "calories_kcal", "tss", "rpe", "notes"
]

# Platform-specific column mappings: {canonical: [possible source names]}
PLATFORM_MAPS = {
    "strava": {
        "date": ["Activity Date", "Start Date", "date"],
        "activity_type": ["Activity Type", "sport_type", "type"],
        "duration_min": ["Moving Time", "Elapsed Time", "moving_time"],
        "distance_km": ["Distance", "distance"],
        "avg_hr": ["Average Heart Rate", "average_heartrate"],
        "max_hr": ["Max Heart Rate", "max_heartrate"],
        "avg_power_w": ["Average Watts", "average_watts"],
        "max_power_w": ["Max Watts", "max_watts"],
        "calories_kcal": ["Calories", "calories"],
        "tss": ["Training Stress Score®", "suffer_score"],
        "rpe": [],
        "notes": ["Activity Description", "description"],
    },
    "whoop": {
        "date": ["Date", "Cycle start time"],
        "activity_type": ["Activity", "Sport"],
        "duration_min": ["Activity duration (min)", "Duration (min)"],
        "distance_km": [],
        "avg_hr": ["Average Heart Rate", "Avg HR"],
        "max_hr": ["Max Heart Rate", "Max HR"],
        "avg_power_w": [],
        "max_power_w": [],
        "calories_kcal": ["Kilojoules", "Calories"],
        "tss": [],
        "rpe": ["Perceived exertion", "RPE"],
        "notes": ["Notes"],
    },
    "garmin": {
        "date": ["Date", "Activity Date", "Timestamp"],
        "activity_type": ["Activity Type", "Title"],
        "duration_min": ["Time", "Moving Time", "Elapsed Time"],
        "distance_km": ["Distance", "Distance (km)"],
        "avg_hr": ["Avg HR", "Average HR"],
        "max_hr": ["Max HR"],
        "avg_power_w": ["Avg Power", "Average Power"],
        "max_power_w": ["Max Power"],
        "calories_kcal": ["Calories", "Total Calories"],
        "tss": ["Training Stress Score", "TSS"],
        "rpe": [],
        "notes": ["Notes", "Title"],
    }
}

def strip_trademarks(df: pd.DataFrame) -> pd.DataFrame:
    """
    Strip registered trademark (®) and trademark (™) symbols from column headers.
    Garmin exports frequently include these (e.g., 'Normalized Power® (NP®)', 'Training Stress Score®')
    which cause KeyError exceptions in pandas if not cleaned first.
    """
    import re
    df.columns = [re.sub(r'[®™]', '', col).strip() for col in df.columns]
    return df

def normalize_intervals_icu_dates(df: pd.DataFrame) -> pd.DataFrame:
    """
    Intervals.icu wellness CSVs occasionally format dates as YYYY-DD-MM.
    Detect and recast to ISO standard YYYY-MM-DD before time-series analysis.
    """
    if "date" not in [c.lower() for c in df.columns]:
        return df
    date_col = next(c for c in df.columns if c.lower() == "date")
    try:
        sample = str(df[date_col].iloc[0])
        parts = sample.split("-")
        # If middle part > 12, it's a day value — format is YYYY-DD-MM
        if len(parts) == 3 and int(parts[1]) > 12:
            print(f"[APEX] Detected non-ISO date format (YYYY-DD-MM) — recasting to YYYY-MM-DD")
            df[date_col] = df[date_col].apply(
                lambda x: f"{str(x)[:4]}-{str(x)[8:10]}-{str(x)[5:7]}" if pd.notna(x) else x
            )
    except (ValueError, IndexError):
        pass
    return df

def detect_platform(df: pd.DataFrame) -> str:
    """Heuristically detect CSV source platform."""
    cols = set(df.columns.str.lower())
    if any(c in cols for c in ["activity type", "strava"]):
        return "strava"
    if any(c in cols for c in ["cycle start time", "recovery score %", "whoop"]):
        return "whoop"
    if any(c in cols for c in ["avg power", "avg hr", "garmin"]):
        return "garmin"
    if any(c in cols for c in ["restinghr", "hrv", "spo2", "ctl", "atl"]):
        return "intervals_icu"
    return "unknown"

def map_columns(df: pd.DataFrame, platform: str) -> pd.DataFrame:
    """Map source columns to canonical schema."""
    mapping = PLATFORM_MAPS.get(platform, {})
    out = pd.DataFrame()

    for canonical, candidates in mapping.items():
        matched = False
        for candidate in candidates:
            # Case-insensitive match
            matching_cols = [c for c in df.columns if c.strip().lower() == candidate.strip().lower()]
            if matching_cols:
                out[canonical] = df[matching_cols[0]]
                matched = True
                break
        if not matched:
            out[canonical] = None

    return out

def normalize_duration(df: pd.DataFrame) -> pd.DataFrame:
    """Convert HH:MM:SS strings to float minutes where needed."""
    if "duration_min" in df.columns:
        def parse_dur(val):
            if pd.isna(val):
                return None
            val = str(val)
            if ":" in val:
                parts = val.split(":")
                try:
                    if len(parts) == 3:
                        return int(parts[0]) * 60 + int(parts[1]) + int(parts[2]) / 60
                    elif len(parts) == 2:
                        return int(parts[0]) + int(parts[1]) / 60
                except ValueError:
                    return None
            try:
                return float(val) / 60  # assume seconds
            except ValueError:
                return None
        df["duration_min"] = df["duration_min"].apply(parse_dur)
    return df

def apply_rowing_watt_normalization(df: pd.DataFrame) -> pd.DataFrame:
    """
    APEX Interference Rule: Rowing watts are metabolically ~12% more costly.
    Add a normalized_cycling_equiv_w column for cross-modal TSS calculation.
    Default factor: 1.12. Overridden by personal_calibration_factor if computed.
    """
    factor = 1.12
    factor_source = "DEFAULT (population heuristic — insufficient paired data for personal calibration)"

    # Attempt personal calibration if multi-modal history exists
    if all(c in df.columns for c in ["avg_power_w", "activity_type", "avg_hr"]):
        rowing_rows = df[df["activity_type"].str.lower().str.contains("row|erg", na=False)]
        cycling_rows = df[df["activity_type"].str.lower().str.contains("cycl|bike|ride", na=False)]
        if len(rowing_rows) >= 5 and len(cycling_rows) >= 5:
            # Match sessions by closest HR average (within ±5 bpm)
            paired_ratios = []
            for _, r_row in rowing_rows.iterrows():
                if pd.isna(r_row["avg_hr"]) or pd.isna(r_row["avg_power_w"]):
                    continue
                match = cycling_rows[
                    (cycling_rows["avg_hr"] - r_row["avg_hr"]).abs() <= 5
                ]
                if not match.empty and not pd.isna(match.iloc[0]["avg_power_w"]):
                    ratio = match.iloc[0]["avg_power_w"] / r_row["avg_power_w"]
                    paired_ratios.append(ratio)
            if len(paired_ratios) >= 3:
                factor = round(sum(paired_ratios) / len(paired_ratios), 3)
                factor_source = f"PERSONAL CALIBRATION ({len(paired_ratios)} matched session pairs)"

    print(f"[APEX] Watt normalization factor: {factor} [{factor_source}]")

    if "avg_power_w" in df.columns and "activity_type" in df.columns:
        def normalize(row):
            activity = str(row.get("activity_type", "")).lower()
            power = row.get("avg_power_w")
            if pd.isna(power):
                return None
            if "row" in activity or "erg" in activity:
                return round(float(power) * factor, 1)
            return float(power)
        df["normalized_cycling_equiv_w"] = df.apply(normalize, axis=1)
        df["watt_normalization_factor"] = factor
    return df

def main():
    parser = argparse.ArgumentParser(description="APEX CSV Schema Mapper")
    parser.add_argument("--file", required=True, help="Path to input CSV file")
    parser.add_argument("--output", default=None, help="Path to save mapped CSV (optional)")
    parser.add_argument("--json", action="store_true", help="Output as JSON instead of table")
    args = parser.parse_args()

    try:
        df = pd.read_csv(args.file)
    except Exception as e:
        print(f"ERROR: Could not read CSV — {e}", file=sys.stderr)
        sys.exit(1)

    # Pre-processing: strip trademarks before any column matching
    df = strip_trademarks(df)
    # Normalize Intervals.icu date quirks
    df = normalize_intervals_icu_dates(df)

    platform = detect_platform(df)

    if platform == "unknown":
        print("[APEX] ⚠️  Platform detection confidence below threshold.")
        print(f"[APEX] Detected columns: {list(df.columns[:10])}{'...' if len(df.columns) > 10 else ''}")
        print("[APEX] ACTION REQUIRED: Please confirm column mappings manually.")
        print("[APEX] Map the following canonical fields to your source columns:")
        for field in CANONICAL_COLUMNS:
            print(f"  {field}: ???")
        print("[APEX] Provide mapping as --platform strava|whoop|garmin|intervals_icu or add manual mapping support.")
        sys.exit(2)
    print(f"[APEX] Detected platform: {platform.upper()}")
    print(f"[APEX] Source columns: {list(df.columns)}")

    mapped = map_columns(df, platform)
    mapped = normalize_duration(mapped)
    mapped = apply_rowing_watt_normalization(mapped)

    print(f"[APEX] Mapped {len(mapped)} rows × {len(mapped.columns)} canonical columns\n")

    if args.json:
        print(mapped.to_json(orient="records", indent=2))
    else:
        print(mapped.to_string(index=False))

    if args.output:
        mapped.to_csv(args.output, index=False)
        print(f"\n[APEX] Saved mapped CSV → {args.output}")

    return mapped

if __name__ == "__main__":
    main()
