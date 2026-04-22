# Homework 27 — Solution
**CPSC 375: High-Performance Computing**

---

## Question 1 — Amdahl's Law (Formal Proof)

**Problem:** Give a formal proof of Amdahl's Law. Let T_s be execution time on 1 process, T_p be execution time on p processes. Assume a fraction f of the program is serial and the rest is perfectly parallel. Express S_p in terms of f and p, then compute S_p = T_s / T_p.

---

### Proof

Let:
- T_s = total execution time on 1 processor
- f = fraction of the program that is **serial** (0 ≤ f ≤ 1)
- (1 − f) = fraction that is **perfectly parallelizable**
- p = number of processors

**Step 1: Model T_p**

The serial portion takes the same time regardless of p:
```
Serial time = f · T_s
```

The parallel portion is divided evenly among p processors:
```
Parallel time = (1 − f) · T_s / p
```

Therefore:
```
T_p = f · T_s + (1 − f) · T_s / p
```

**Step 2: Compute Speedup S_p = T_s / T_p**

```
S_p = T_s / T_p
    = T_s / [f · T_s + (1 − f) · T_s / p]
    = 1 / [f + (1 − f) / p]
```

**Amdahl's Law:**

$$S_p = \frac{1}{f + \dfrac{1-f}{p}}$$

**Step 3: Maximum speedup as p → ∞**

```
lim_{p→∞} S_p = lim_{p→∞} 1 / [f + (1−f)/p]
              = 1 / [f + 0]
              = 1/f
```

**Conclusion:** No matter how many processors are used, the speedup is bounded above by **1/f**. Even a 5% serial fraction (f = 0.05) limits maximum speedup to 20×, regardless of how many processors are added.

---

## Question 2 — Gustafson's Law (Formal Proof)

**Problem:** Give a formal proof of Gustafson's Law. Assume T_p = 1 and split it into serial f and parallel (1−f). Convert the parallel portion to sequential work p(1−f), form T_s = f + p(1−f), then compute S_p = T_s / T_p.

---

### Proof

Gustafson's Law models **scaled speedup** — the problem size grows with p so that parallel execution time stays constant.

**Step 1: Fix T_p = 1** (normalize parallel runtime to 1)

With p processors, the parallel execution time has:
- Serial component: f (takes time f regardless of p)
- Parallel component: (1 − f) (this portion runs in time 1−f using p processors)

**Step 2: Reconstruct T_s (equivalent sequential time)**

If we ran the same workload on 1 processor, the parallel portion would take p times as long (since p processors were sharing it):

```
T_s = f + p · (1 − f)
```

**Step 3: Compute Scaled Speedup S_p = T_s / T_p**

```
S_p = T_s / T_p
    = [f + p(1 − f)] / 1
    = f + p(1 − f)
    = f + p − p·f
    = p − f(p − 1)
    = p(1 − f) + f
```

**Gustafson-Barsis Law:**

$$S_p = p - f(p - 1)$$

**Key observation:** As p → ∞, S_p → ∞ (unbounded growth), provided f < 1.

---

## Question 3

**Problem:** Both laws come from the same general speedup formula. Why does Amdahl's Law predict speedup converging to 1/f, while Gustafson-Barsis Law predicts speedup growing without bound? Explain why this is so.

---

### Solution

Both laws use the formula S_p = T_s / T_p, but they differ in **what is held constant as p increases**:

**Amdahl's Law — fixed problem size:**
- T_s is fixed (the total sequential work stays the same)
- As p increases, the parallel portion shrinks proportionally
- The serial fraction f · T_s is an **absolute time cost** that cannot be reduced
- Eventually, the serial portion dominates and adding more processors yields diminishing returns
- **Ceiling: S_p → 1/f**

**Gustafson's Law — fixed parallel execution time:**
- T_p = 1 is fixed (we always keep processors busy for the same wall-clock time)
- As p increases, we solve a **proportionally larger problem** in the same time
- The amount of useful parallel work grows as p(1−f), but the serial work f stays constant
- The serial fraction f represents a **fixed overhead** relative to a growing total workload
- As the total work grows, f becomes a smaller and smaller *fraction* of T_s, so speedup scales linearly
- **Unbounded: S_p = p − f(p−1) → ∞**

**The fundamental difference** is whether we model a fixed or scaled workload. Amdahl's Law is pessimistic (relevant when problem size is fixed). Gustafson's Law is optimistic (relevant in real scientific computing, where we use more processors to solve bigger problems in the same time).

---

## Question 4 — Karp-Flatt Metric

**Problem:** The Karp-Flatt metric estimates how much of a program is effectively serial based on observed speedup. Derive an expression for the experimentally determined serial fraction. Hint: invert speedup, then isolate the serial term.

---

### Derivation

Start from Amdahl's formula:

```
S_p = 1 / [f + (1−f)/p]
```

**Step 1: Invert both sides**

```
1/S_p = f + (1−f)/p
```

**Step 2: Expand the right-hand side**

```
1/S_p = f + 1/p − f/p
```

**Step 3: Subtract 1/p from both sides**

```
1/S_p − 1/p = f − f/p = f(1 − 1/p)
```

**Step 4: Solve for f**

```
f = (1/S_p − 1/p) / (1 − 1/p)
```

**Karp-Flatt Metric:**

$$e = \frac{\dfrac{1}{S_p} - \dfrac{1}{p}}{1 - \dfrac{1}{p}}$$

where e is the experimentally determined serial fraction.

**Interpretation:**
- If e is **constant** as p increases → the bottleneck is a fixed serial portion; Amdahl's Law applies; adding more processors won't help much.
- If e **increases** as p increases → the overhead is growing (e.g., communication costs, synchronization, or load imbalance), which is not predicted by Amdahl's Law alone. This signals a scalability problem beyond simple serial code.

---

## Question 5 — Speedup Table Analysis

**Problem:** Six parallel programs (I–VI) have been benchmarked on 1–8 processors. For each program, choose the statement that best describes its likely performance on 16 processors:

- A. Speedup on 16 will be ≥ 40% higher than on 8
- B. Speedup on 16 will be < 40% higher than speedup on 8, due to **large serial component**
- C. Speedup on 16 will be < 40% higher than speedup on 8, due to **increasing overhead**

| Proc | I    | II   | III  | IV   | V    | VI   |
|------|------|------|------|------|------|------|
| 1    | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 |
| 2    | 1.67 | 1.89 | 1.89 | 1.96 | 1.74 | 1.94 |
| 3    | 2.14 | 2.63 | 2.68 | 2.88 | 2.30 | 2.82 |
| 4    | 2.50 | 3.23 | 3.39 | 3.67 | 2.74 | 3.65 |
| 5    | 2.78 | 3.68 | 4.03 | 4.46 | 3.09 | 4.42 |
| 6    | 3.00 | 4.00 | 4.62 | 5.22 | 3.38 | 5.15 |
| 7    | 3.18 | 4.22 | 5.15 | 5.93 | 3.62 | 5.84 |
| 8    | 3.33 | 4.35 | 5.63 | 6.25 | 3.81 | 6.50 |

---

### Analysis Using the Karp-Flatt Metric

Compute e = (1/S_p − 1/p) / (1 − 1/p) for each program at p = 2, 4, 8 to detect trends:

**Program I:**
- p=2: e = (1/1.67 − 0.5)/(0.5) = (0.599 − 0.5)/0.5 = 0.199
- p=4: e = (1/2.50 − 0.25)/(0.75) = (0.400 − 0.25)/0.75 = 0.200
- p=8: e = (1/3.33 − 0.125)/(0.875) = (0.300 − 0.125)/0.875 = 0.200

e is **constant** ≈ 0.20. Pure serial bottleneck. S_8 = 3.33, and with f=0.20, S_16 = 1/(0.20 + 0.80/16) = 1/0.25 = **4.00**. Ratio: 4.00/3.33 = 1.20 → **20% increase, well below 40%.**

**→ Program I: Answer B** (large serial component, f ≈ 20%)

---

**Program II:**
- p=2: e = (1/1.89 − 0.5)/0.5 = (0.529 − 0.5)/0.5 = 0.058
- p=4: e = (1/3.23 − 0.25)/0.75 = (0.310 − 0.25)/0.75 = 0.080
- p=8: e = (1/4.35 − 0.125)/0.875 = (0.230 − 0.125)/0.875 = 0.120

e is **increasing** (0.058 → 0.080 → 0.120). Growing overhead. S_8 = 4.35; 40% higher would be 6.09. Given increasing overhead, the real S_16 will fall short.

**→ Program II: Answer C** (increasing overhead as processors are added)

---

**Program III:**
- p=2: e = (1/1.89 − 0.5)/0.5 = 0.058
- p=4: e = (1/3.39 − 0.25)/0.75 = (0.295 − 0.25)/0.75 = 0.060
- p=8: e = (1/5.63 − 0.125)/0.875 = (0.178 − 0.125)/0.875 = 0.061

e is **nearly constant** at ~0.06. S_8 = 5.63; S_16 ≈ 1/(0.06 + 0.94/16) = 1/(0.06 + 0.059) = 1/0.119 ≈ **8.40**. Ratio: 8.40/5.63 = 1.49 → **~49% increase, above 40%.**

**→ Program III: Answer A** (consistent low serial fraction, likely to continue scaling well)

---

**Program IV:**
- p=2: e = (1/1.96 − 0.5)/0.5 = (0.510 − 0.5)/0.5 = 0.020
- p=4: e = (1/3.67 − 0.25)/0.75 = (0.272 − 0.25)/0.75 = 0.030
- p=8: e = (1/6.25 − 0.125)/0.875 = (0.160 − 0.125)/0.875 = 0.040

e is **slowly increasing** (0.02 → 0.03 → 0.04). Very low serial fraction but mild overhead growth. S_8 = 6.25; 40% more = 8.75. With e ≈ 0.05 at p=16: S_16 ≈ 1/(0.05 + 0.95/16) ≈ 1/0.109 ≈ 9.17 → 9.17/6.25 = 1.47.

**→ Program IV: Answer A** (very low serial fraction, strong scaling expected)

---

**Program V:**
- p=2: e = (1/1.74 − 0.5)/0.5 = (0.575 − 0.5)/0.5 = 0.149
- p=4: e = (1/2.74 − 0.25)/0.75 = (0.365 − 0.25)/0.75 = 0.153
- p=8: e = (1/3.81 − 0.125)/0.875 = (0.263 − 0.125)/0.875 = 0.158

e is **slightly but consistently increasing** (0.149 → 0.153 → 0.158). Primarily a large serial component with mild overhead. S_8 = 3.81; S_16 ≈ 1/(0.16 + 0.84/16) = 1/(0.16 + 0.0525) = 1/0.2125 ≈ 4.71. Ratio: 4.71/3.81 = 1.24.

**→ Program V: Answer B** (large serial component dominates; f ≈ 15–16%)

---

**Program VI:**
- p=2: e = (1/1.94 − 0.5)/0.5 = (0.515 − 0.5)/0.5 = 0.031
- p=4: e = (1/3.65 − 0.25)/0.75 = (0.274 − 0.25)/0.75 = 0.032
- p=8: e = (1/6.50 − 0.125)/0.875 = (0.154 − 0.125)/0.875 = 0.033

e is **essentially constant** ≈ 0.032. Very low, stable serial fraction. S_8 = 6.50; S_16 ≈ 1/(0.033 + 0.967/16) = 1/(0.033 + 0.060) = 1/0.093 ≈ 10.75. Ratio: 10.75/6.50 = 1.65 → **65% increase, well above 40%.**

**→ Program VI: Answer A** (very small constant serial fraction, excellent scalability)

---

### Summary

| Program | Answer | Reasoning |
|---------|--------|-----------|
| I   | **B** | e ≈ constant 0.20 — large serial fraction (~20%) caps speedup |
| II  | **C** | e increasing 0.06→0.12 — growing communication/overhead |
| III | **A** | e ≈ constant 0.06 — low serial fraction, predicts >40% gain |
| IV  | **A** | e ≈ 0.02–0.04 — nearly ideal parallelism, strong scaling |
| V   | **B** | e ≈ constant 0.15 — large serial fraction limits gains |
| VI  | **A** | e ≈ constant 0.03 — excellent scalability, predicts >60% gain |
