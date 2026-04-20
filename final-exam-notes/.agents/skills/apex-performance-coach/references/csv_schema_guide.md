# APEX CSV Schema Guide

Reference for known column name variants across Strava, Whoop, and Garmin exports.
Used by `scripts/csv_mapper.py` to normalize to the canonical schema.

## Canonical Schema

| Field | Type | Description |
|---|---|---|
| `date` | string/datetime | Session date |
| `activity_type` | string | e.g., "Rowing", "Cycling", "Run", "Alpine Ski" |
| `duration_min` | float | Duration in minutes |
| `distance_km` | float | Distance in kilometers |
| `avg_hr` | int | Average heart rate (bpm) |
| `max_hr` | int | Max heart rate (bpm) |
| `avg_power_w` | float | Average power (watts) |
| `max_power_w` | float | Peak power (watts) |
| `calories_kcal` | int | Total caloric expenditure |
| `tss` | float | Training Stress Score (if available) |
| `rpe` | int | Rate of Perceived Exertion (1–10) |
| `notes` | string | Activity description or notes |

---

## Strava CSV Export

Accessed via: Settings → My Account → Download or Delete Your Account → Request Your Archive

Key columns:
```
Activity ID, Activity Date, Activity Name, Activity Type, Activity Description,
Elapsed Time, Distance, Moving Time, Max Speed, Average Speed,
Elevation Gain, Elevation Loss, Elevation Low, Elevation High,
Max Grade, Average Grade, Max Cadence, Average Cadence,
Max Heart Rate, Average Heart Rate, Calories, Max Watts, Average Watts,
Perceived Exertion, Training Stress Score®
```

**Duration note:** Strava exports "Moving Time" and "Elapsed Time" in seconds — divide by 60.
**Distance note:** Strava distance is in kilometers by default, but may be miles depending on user settings — check header.

---

## Whoop CSV Export

Accessed via: Whoop app → Profile → Account Settings → Export Data

Activity file columns:
```
Cycle start time, Cycle end time, Sleep duration (min), Sleep performance %,
Respiratory rate, Resting heart rate, HRV (ms), Recovery score %,
Activity, Activity start, Activity end, Activity duration (min),
Average Heart Rate, Max Heart Rate, Kilojoules
```

**Key insight:** Whoop separates Recovery (daily cycle) and Activity (workout) data into different CSV files.
- Recovery data → use for RRS computation (HRV, RHR, sleep)
- Activity data → use for session load

---

## Garmin CSV Export

Accessed via: Garmin Connect → Activities → Export CSV
or via: Garmin Connect Export All Data

Key columns:
```
Activity Type, Date, Favorite, Title, Distance, Calories,
Time, Avg HR, Max HR, Aerobic TE, Avg Run Cadence, Max Run Cadence,
Avg Pace, Best Pace, Total Ascent, Total Descent, Avg Stride Length,
Avg Vertical Ratio, Avg Vertical Oscillation, Avg Ground Contact Time,
Training Stress Score, Avg Power, Max Power, Normalized Power (NP),
Left Balance, Right Balance, Avg Left Pedal Smoothness, Avg Right Pedal Smoothness,
Avg Combined Pedal Smoothness, Avg Fractional Cadence
```

**Duration note:** Garmin "Time" field is in HH:MM:SS format — parse accordingly.
**Power note:** Garmin distinguishes "Avg Power", "Max Power", and "Normalized Power (NP)" — prefer NP for TSS calculation if available.

---

## Rowing-Specific Notes

- Concept2 ErgData exports (CSV/JSON) include: `date`, `time`, `distance`, `pace_500m`, `watts`, `calories`, `stroke_rate`, `heart_rate`
- Pace should be in MM:SS.s per 500m format
- **Always apply the 1.12× watt normalization** when comparing rowing power to cycling power
- Erg 2k split → water pace: add 4–6 seconds depending on crew and conditions

---

## Alpine Ski Detection

Watch for these activity_type values to trigger the ski override rule:
- "Alpine Ski", "Backcountry Ski", "Ski", "Skiing", "Ski Touring", "Ski Mountaineering"
- Garmin: "Backcountry Skiing/Snowboarding"
- Strava: "AlpineSki", "BackcountrySki", "NordicSki"

**Rule:** If total duration_min for ski activities in last 48h > 240 minutes (4 hours), halve gym volume for 48h.
