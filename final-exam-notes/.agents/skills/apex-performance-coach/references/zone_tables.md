# APEX Zone Reference Tables

## Heart Rate Zones

### Rowing (HRmax-based)
| Zone | Name | % HRmax | Typical Feel | Use Case |
|---|---|---|---|---|
| Z1 | Active Recovery | <60% | Very easy, conversational | Cool-down, recovery rows |
| Z2 | Aerobic Base | 60–70% | Easy, can hold full sentences | Long steady state, L4 pieces |
| Z3 | Tempo | 70–80% | Moderate, broken sentences | Race pace training, 6k-16k |
| Z4 | Threshold | 80–90% | Hard, few words | 4×2k, threshold intervals |
| Z5 | VO2max/Anaerobic | 90–100% | All-out, unsustainable | 2k race, sprint pieces |

**Sean's estimated HRmax:** ~195 bpm (D3 collegiate rower, 20yo male)
**Derived zones:**
- Z1: <117 bpm
- Z2: 117–137 bpm
- Z3: 137–156 bpm
- Z4: 156–175 bpm
- Z5: 175–195 bpm

---

### Cycling (FTP-based, % FTP)
| Zone | Name | % FTP | Use Case |
|---|---|---|---|
| Z1 | Active Recovery | <55% | Easy spin, recovery |
| Z2 | Endurance | 56–75% | Long aerobic rides |
| Z3 | Tempo | 76–90% | Sustained effort, 20–60 min |
| Z4 | Threshold | 91–105% | FTP intervals, 10–20 min |
| Z5 | VO2max | 106–120% | 3–8 min intervals |
| Z6 | Anaerobic | 121–150% | 30s–2 min bursts |
| Z7 | Neuromuscular | >150% | Sprints, <30s |

---

## Rowing Pace Zones (500m split)

Based on Sean's approximate current fitness (adjust as 2k time evolves):

| Zone | 500m Split | Effort |
|---|---|---|
| Z1/Recovery | >2:15 | Very easy |
| Z2/Steady State | 2:00–2:15 | Aerobic base |
| Z3/Tempo | 1:52–2:00 | Race-adjacent |
| Z4/Threshold | 1:45–1:52 | Threshold |
| Z5/Race | <1:45 | 2k race pace and faster |

---

## Watt Normalization Reference

### Rowing → Cycling Metabolic Equivalent
```
Normalized Cycling Equiv = Rowing Watts × 1.12
```
Example: 200W on erg = 224W equivalent cycling workload

### Cross-Modal TSS Calculation
```
IF (Intensity Factor) = Normalized Power / FTP  [cycling]
IF (Intensity Factor) = Avg Pace / Threshold Pace  [rowing, pace-based]

TSS = (Duration_sec × NP × IF) / (FTP × 3600) × 100
```

For rowing, use pace-based IF. For cycling, use power-based IF. Normalize before comparing daily TSS across modalities.

---

## Training Stress Score (TSS) Guidelines

| TSS | Fatigue Level | Recovery Time |
|---|---|---|
| <50 | Low | <24h |
| 50–100 | Moderate | 24h |
| 100–150 | High | 24–36h |
| 150–200 | Very High | 36–48h |
| >200 | Extreme | 48–72h |

**Weekly TSS targets (approximate, adjust for phase):**
- Base/Build: 400–600
- Race prep: 300–450
- Taper week: 150–250
- Recovery week: 100–200
