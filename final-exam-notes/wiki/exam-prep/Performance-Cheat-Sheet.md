---
aliases: [HPC Formulas, Performance Metrics Sheet]
tags: [#exam/formula, #prep/moc, #course_hpc]
sources: [lec17.md, lec27.md, lec28.md, Homework-27-Solutions.md, Homework-28-Solutions.md]
created: 2026-04-20
updated: 2026-04-20
---

# Cheat Sheet: Performance Analysis

> [!abstract]
> Condensed formulas and scaling laws for speedup, efficiency, and scalability diagnosis.

## 1. Fundamental Metrics
> [!equation]
> - **Speedup ($S_p$):** $$S_p = \frac{T_s}{T_p}$$
> - **Efficiency ($E$):** $$E = \frac{S_p}{p} = \frac{T_s}{p \cdot T_p}$$
> - **Parallel Time Model:** $$T_p = \frac{T_s}{p} + T_o$$ ($T_o$ is total overhead)

## 2. Scaling Laws

| Feature | Amdahl's Law | Gustafson's Law |
| :--- | :--- | :--- |
| **Scaling Type** | **Strong Scaling** | **Weak Scaling** |
| **Problem Size** | Fixed | Scaled with $p$ |
| **Goal** | Minimize Latency ($T_p$) | Maximize Work ($W$) |
| **Speedup Limit**| $1/f$ (Plateau) | Unbounded (Linear) |
| **Typical Use** | Real-time / Deadlines | Scientific Simulations |

## 3. Diagnostic Metrics
> [!equation] Karp-Flatt Metric ($e$)
> Identifies if bottleneck is **Serial Fraction** or **Overhead**.
> $$e = \frac{1/S_p - 1/p}{1 - 1/p}$$
> - **Constant $e$:** Problem is the algorithm's serial code.
> - **Increasing $e$:** Problem is implementation overhead (comm/sync).

> [!equation] Isoefficiency Function ($W$)
> Rate work must grow to maintain constant efficiency.
> $$W = K \cdot p T_o(p, W)$$ where $K = \frac{E}{1-E}$
> - **$O(p)$:** Highly Scalable.
> - **$O(p \log p)$:** Scalable.
> - **$O(p^2)$:** Poorly Scalable.

## 4. Hardware/Algorithm Limits
> [!info] Scalability Requirement
> For complex algorithms like matrix multiplication to scale well, the **Computation-to-Communication Ratio** must be high.
> - **Checkerboard Ratio**: $\frac{n}{\sqrt{p}} \gg 1$
> - **Implication**: $n$ must grow with $\sqrt{p}$ to maintain constant efficiency.

> [!warning] Common Pitfalls
> [!warning]
> - **Linear Speedup:** $S_p = p$. Rarely achieved due to $T_o$.
> - **Superlinear Speedup:** $S_p > p$. Usually due to aggregate cache size increases.
> - **Idle Time:** Caused by load imbalance or synchronization barriers.
