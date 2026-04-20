---
aliases: [Speedup, Sublinear Speedup]
tags: [#exam/formula, performance]
sources: [lec17.pdf, lec27.txt]
created: 2025-05-15
updated: 2026-04-20
---

# Parallel Speedup

> [!abstract] TL;DR Summary
> A measure of how much faster a parallel algorithm runs on $p$ processors compared to the best sequential algorithm. **Linear Speedup** ($S_p = p$) is the ideal case, representing a perfectly scalable system.

## Core Mechanics

> [!equation] Speedup Formula
> $$S_p = \frac{T_s}{T_p}$$
> - $T_s$ = Sequential execution time (best serial algorithm)
> - $T_p$ = Parallel execution time on $p$ processors

### Types of Speedup
* **Linear Speedup:** $S_p = p$. The "Gold Standard" of scaling.
* **Sublinear Speedup:** $S_p < p$. Most common; caused by communication overhead, synchronization, and serial sections.
* **Superlinear Speedup:** $S_p > p$. Usually occurs when the total problem fits into the combined cache of multiple processors, reducing memory access latency.

> [!warning] Load Imbalance
> If work is not distributed evenly, $T_p$ will be determined by the slowest process, significantly reducing $S_p$.

## Connections
* **Component of:** [[parallel-efficiency]].
* **Theories:** [[amdahls-law]] (Fixed), [[gustafsons-law]] (Scaled).
* **Diagnosed By:** [[karp-flatt-metric]].
* **Measured In:** [[strong-scalability]], [[weak-scalability]].
