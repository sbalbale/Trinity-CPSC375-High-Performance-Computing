---
aliases: [Homework 27 Solutions]
tags: [#homework/solutions, #course_hpc, #performance]
sources: [Homework 27.pdf, lec27.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Homework 27 Solutions

> [!abstract]
> Solutions to the parallel performance analysis exercises for Homework 27, featuring formal proofs of scaling laws and diagnostic analysis of program speedup data.

## Problem 1: Formal Proof of Amdahl's Law

**Goal:** Express $S_p$ in terms of serial fraction $f$ and processor count $p$.

**Proof:**
1. Let $T_s$ be the execution time on 1 process. Let $T_s = 1$.
2. The serial fraction $f$ of the program takes $f \cdot T_s$ time.
3. The parallelizable fraction $(1-f)$ takes $(1-f) \cdot T_s$ time.
4. On $p$ processors, the parallel part is divided by $p$, but the serial part remains unchanged.
5. Parallel execution time: $T_p = f \cdot T_s + \frac{(1-f) \cdot T_s}{p}$
6. Since $T_s = 1$, we have: $T_p = f + \frac{1-f}{p}$
7. Speedup $S_p = \frac{T_s}{T_p} = \frac{1}{f + \frac{1-f}{p}}$

> [!equation] Amdahl's Law
> $$S_p = \frac{1}{f + \frac{1-f}{p}}$$

---

## Problem 2: Formal Proof of Gustafson's Law

**Goal:** Express $S_p$ in terms of $f$ and $p$ for scaled problem sizes.

**Proof:**
1. Assume parallel execution time $T_p = 1$.
2. In this unit of time, the serial part takes $f$ and the parallel part takes $1-f$.
3. Total sequential work $T_s$ required to perform the same task:
    - The serial part takes $f$.
    - The parallel part (which took $1-f$ on $p$ processors) would take $p \cdot (1-f)$ on a single processor.
4. $T_s = f + p(1-f)$
5. Speedup $S_p = \frac{T_s}{T_p} = \frac{f + p(1-f)}{1} = f + p - pf$
6. Rearranging: $S_p = p + (1-p)f$

> [!equation] Gustafson's Law
> $$S_p = p + (1-p)f$$

---

## Problem 3: Convergence vs. Unbounded Growth

**Amdahl's Law (Fixed Size):**
As $p \to \infty$, the term $\frac{1-f}{p} \to 0$. The speedup $S_p$ converges to **$1/f$**. This is because for a fixed problem, the serial component eventually becomes the absolute bottleneck that prevents further speedup.

**Gustafson's Law (Scaled Size):**
$S_p = p + (1-p)f$. As $p \to \infty$, $S_p \to \infty$. This is because the workload is allowed to increase with the number of processors. The parallel work grows relative to the serial work, allowing the system to maintain high speedup.

---

## Problem 4: Derivation of Karp-Flatt Metric

**Goal:** Derive $e = \frac{1/S_p - 1/p}{1 - 1/p}$.

**Derivation:**
1. Start with the Amdahl-style speedup formula using the experimental serial fraction $e$:
   $$S_p = \frac{1}{e + \frac{1-e}{p}}$$
2. Invert both sides:
   $$\frac{1}{S_p} = e + \frac{1-e}{p}$$
3. Expand:
   $$\frac{1}{S_p} = e + \frac{1}{p} - \frac{e}{p}$$
4. Group $e$ terms:
   $$\frac{1}{S_p} - \frac{1}{p} = e\left(1 - \frac{1}{p}\right)$$
5. Solve for $e$:
   $$e = \frac{\frac{1}{S_p} - \frac{1}{p}}{1 - \frac{1}{p}}$$

---

## Problem 5: Program Benchmarking Analysis

Using the Karp-Flatt metric $e = \frac{1/S_p - 1/p}{1 - 1/p}$, we analyze the speedup data:

| Program | $S_2$ | $S_8$ | $e_2$ | $e_8$ | Trend | Conclusion |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **I** | 1.67 | 3.33 | 0.20 | 0.20 | Constant | **B** (Serial bottleneck) |
| **II** | 1.89 | 4.35 | 0.06 | 0.12 | Increasing | **C** (Overhead bottleneck) |
| **III** | 1.89 | 5.63 | 0.06 | 0.06 | Constant | **A** ($S_{16} \approx 8.4$ > 7.8) |
| **IV** | 1.96 | 6.25 | 0.02 | 0.04 | Sl. Incr | **B** ($S_{16} \approx 10.0$ > 8.7) |
| **V** | 1.74 | 3.81 | 0.15 | 0.16 | Constant | **B** (Serial bottleneck) |
| **VI** | 1.94 | 6.50 | 0.03 | 0.03 | Constant | **A** ($S_{16} \approx 11.1$ > 9.1) |

### Choices Defined:
- **A:** Significant speedup (> 40% gain). $S_{16} \ge 1.4 \times S_8$.
- **B:** Modest speedup (< 40% gain) due to **serial fraction**.
- **C:** Poor speedup (< 40% gain) due to **parallel overhead**.

**Summary Analysis:**
- **Programs III, IV, VI** show very low and relatively stable $e$ values. Extrapolating to 16 processors results in speedups that meet or exceed the 40% threshold.
- **Programs I, V** have high constant $e$, meaning the serial logic is a major constraint. They won't hit the 40% target.
- **Program II** is the only one showing a significant increase in $e$, indicating that overhead is the primary reason for poor scaling.
