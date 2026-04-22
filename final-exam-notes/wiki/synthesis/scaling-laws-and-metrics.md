---
aliases: [Amdahl vs Gustafson, Scaling Laws Synthesis, HPC Performance Metrics]
tags: [#exam/theory, #exam/formula, #synthesis, performance]
sources: [homework-27.md, homework-28.md, lec17.md, lec27.md]
created: 2026-04-21
updated: 2026-04-21
---

# Scaling Laws & Performance Metrics

> [!abstract]
> This synthesis provides a unified framework for predicting and diagnosing parallel performance. It bridges the gap between theoretical limits (**Amdahl's Law**), practical scaling (**Gustafson's Law**), and experimental diagnostics (**Karp-Flatt Metric** and **Isoefficiency**).

## Theoretical Perspectives: Fixed vs. Scaled Workload

| Law | Primary Assumption | Formula | Focus | Speedup Limit |
|---|---|---|---|---|
| **[[amdahls-law]]** | **Fixed** problem size | $S_p = \frac{1}{f + (1-f)/p}$ | Strong Scalability | $\lim_{p\to\infty} S_p = 1/f$ |
| **[[gustafsons-law]]** | **Scaled** problem size | $S_p = p + (1-p)f$ | Weak Scalability | **Unbounded** (O(p)) |

### The "Same Formula" Paradox
Both laws are derived from $S_p = T_s / T_p$. The difference lies in **what is normalized to 1**:
- **Amdahl** assumes $T_s = 1$ (the work is fixed). As you add processors, the parallel time shrinks, but the serial time $f$ becomes a larger *percentage* of the remaining time.
- **Gustafson** assumes $T_p = 1$ (the runtime is fixed). As you add processors, you solve a larger problem. The serial work $f$ becomes a smaller *percentage* of the total work $T_s = f + p(1-f)$.

---

## Experimental Diagnostics

### 1. The [[karp-flatt-metric]] ($e$)
Used to identify **why** a program is scaling poorly.
> [!equation]
> $$e = \frac{1/S_p - 1/p}{1 - 1/p}$$

**Diagnostic Guide:**
- **$e$ is constant**: The bottleneck is the **algorithm's serial fraction**. Adding processors won't help; you must refactor the logic.
- **$e$ is increasing**: The bottleneck is **parallel overhead** (communication, synchronization). Your implementation or hardware interconnect is failing to scale.

### 2. The [[isoefficiency-function]]
Measures the "scalability" of an algorithm. It tells you how much more data ($W$) you must add to maintain the same efficiency ($E$) when you increase processor count ($p$).
> [!equation]
> $$W \ge K \cdot p \cdot T_o(p, W)$$

- **Highly Scalable**: $O(p)$ or $O(p \log p)$.
- **Poorly Scalable**: $O(p^3)$ or $O(p^4)$. If you double your processors, you might need 16x more data to keep them busy.

---

## Synthesis Example: Weather Model
- **Partitioning**: Uses [[domain-decomposition]] of a 3D grid.
- **Strong Scaling (Amdahl)**: If we keep the grid size same but add nodes, speedup is capped by the serial initialization and the surface-area-to-volume ratio of communication.
- **Weak Scaling (Gustafson)**: If we add nodes and increase grid resolution (more cells), we can maintain high efficiency as the serial part stays constant while useful work grows.

## Connections
* **Concepts**: [[strong-scalability]], [[weak-scalability]], [[parallel-efficiency]], [[overhead-parallel-computing]].
* **Entities**: [[gene-amdahl]], [[john-gustafson]].
