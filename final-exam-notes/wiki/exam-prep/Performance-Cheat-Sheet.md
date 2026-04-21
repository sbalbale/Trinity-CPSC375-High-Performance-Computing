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
> [!equation] Amdahl's Law (Strong Scaling)
> **Constraint:** Fixed problem size.
> $$S_p = \frac{1}{f + \frac{1-f}{p}}$$
> **Limit ($p \to \infty$):** $1/f$

> [!equation] Gustafson's Law (Weak Scaling)
> **Constraint:** Problem size grows with $p$.
> $$S_p = p + (1-p)f$$
> **Limit ($p \to \infty$):** $\infty$ (Unbounded)

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

## 4. Common Pitfalls
> [!warning]
> - **Linear Speedup:** $S_p = p$. Rarely achieved due to $T_o$.
> - **Superlinear Speedup:** $S_p > p$. Usually due to aggregate cache size increases.
> - **Idle Time:** Caused by load imbalance or synchronization barriers.
