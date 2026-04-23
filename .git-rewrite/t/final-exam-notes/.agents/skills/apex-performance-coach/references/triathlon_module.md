# APEX Triathlon Module

Read this file when Sean asks about triathlon training, brick workouts, swim/bike/run sessions,
or off-season multisport development (June–August, Weston MA base).

---

## Discipline Role Mapping

| Discipline | Primary Role | Secondary Role |
|---|---|---|
| Swim | Active recovery for lower extremities | Central aerobic stimulus |
| Bike | Primary aerobic volume builder | Leg-sparing erg substitute |
| Run | VO₂max / lactate stimulus | — |

**Swim is not output-primary.** Log it as RECOVERY_METRIC + aerobic load. Do not compute cycling-equivalent watts from swim pace. Parse via HR if available.

---

## Brick Workout Protocols

### Sprint / Olympic Distance Focus
Primary brick session:
- **45–90 min ride** — include threshold intervals (3×8 min Z4, 5 min Z3 between)
- **45-second transition** (T2 simulation — change shoes, no extended rest)
- **10–20 min run** — cadence focus, not pace focus

> **Physiological challenge:** Blood flow redistribution during T2 causes quad heaviness and altered running mechanics. The goal of brick sessions is adapting to this, not racing through it.

### Cadence Bridging Protocol
The single most effective brick adaptation cue:

```
Target running cadence = cycling cadence ± 5 SPM for first 10 minutes off the bike
```

If cycling at 85 RPM → target 80–90 running strides/min off the bike.
This bridges the neuromuscular gap and reduces the "brick legs" sensation window from ~10 min to ~3–5 min with consistent training.

**Cue Sean:** count left-foot strikes for 30 seconds × 2 = cadence. Adjust effort to hit the window, not pace.

### Ironman / Half-Iron Volume (if applicable)
- Long brick: 3–4h ride + 30–60 min run
- TSS will be extreme (200+). Flag immediately. Mandatory 48h recovery window.
- Nutrition during session: 60g carbs/hour on bike from hour 2 onward.

---

## Triathlon-Specific Interference Rules

### Swim + Bike Same Day
- Acceptable if swim is Z1/Z2 and bike is Z2–Z3.
- Flag if both sessions exceed Z3: total CNS load is high despite different muscle groups.

### Bike → Run (Brick) Eccentric Load
- Running off the bike recruits eccentrically loaded quads mid-fatigue.
- If brick run exceeds 30 min at Z4+: apply 24h lower-body gym reduction (–25% volume).

### Rowing + Triathlon Overlap (Spring/Summer Transition)
- Do not attempt high-intensity rowing AND triathlon HIIT in same 48h window.
- Rowing watts normalized to cycling equivalent — total TSS stack must stay under weekly target.
- Read `references/zone_tables.md` for TSS weekly caps.

---

## Transition Zone (T1 / T2) Training Notes

### T1 (Swim → Bike)
- Wetsuit removal time target: <60 seconds with practice.
- Blood flow shift from upper body (swim) to legs: first 5 min on bike feel easy but HR is elevated — do not chase power immediately.
- First 3 min on bike: Z2 spin. Then ramp to race pace.

### T2 (Bike → Run)
- 45-second rule: rack bike, swap shoes, go. No stretching.
- Cadence bridging begins immediately (see above).
- If race distance is Sprint: accept the brick feeling. If Olympic+: it resolves within 1 mile with proper cadence focus.

---

## Nutrition for Triathlon

| Distance | Pre-race carbs | On-course | Post-race |
|---|---|---|---|
| Sprint | 60g, 2h before | Water only | 40g carbs + 25g protein |
| Olympic | 80g, 2.5h before | 1 gel or 30g carbs/h | 60g carbs + 30g protein |
| Half-Iron | 100g, 3h before | 60g carbs/h from hour 2 | Full recovery meal within 45 min |

Culturally adapted post-race options: see SKILL.md Step 4 nutrition table.

---

## Performance Logging for Triathlon

Metrics to extract from Garmin/Strava CSV:
- Swim: duration, HR avg, distance (if GPS-tracked open water)
- Bike: duration, avg power (normalized), avg HR, TSS
- Run: duration, avg pace, avg HR, cadence
- Transition times (if race data — Garmin tri-mode captures T1/T2)

Apply rowing → cycling normalization only if Sean does an erg session on the same day as cycling. Do not apply between swim and bike.
