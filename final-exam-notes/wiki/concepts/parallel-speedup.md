---
aliases: [Speedup]
tags: [#exam/formula, #hpc]
sources: [lec17.pdf]
created: 2025-05-15
updated: 2025-05-15
---

# Parallel Speedup

> [!abstract] TL;DR Summary
> A measure of how much faster a parallel algorithm runs on $p$ processors compared to the best sequential algorithm. **Linear Speedup** ($S_p = p$) is the ideal case.

## Core Mechanics

> [!equation] Speedup Formula
> $$S_p = \frac{T_s}{T_p}$$
> - $T_s$ = Sequential execution time (best serial algorithm)
> - $T_p$ = Parallel execution time on $p$ processors

### Types of Speedup
* **Linear Speedup:** Performance increases linearly with processors.
* **Sublinear Speedup:** Most common; overhead limits performance.
* **Superlinear Speedup:** $S_p > p$; usually caused by increased aggregate cache size (data fits in cache when split across nodes).

> [!warning] Common Pitfalls
> * **Weak vs. Strong Scaling:** Speedup results depend on whether the problem size is fixed or increasing.
> * **Hardware Bottlenecks:** Memory bandwidth or network latency can prevent linear speedup.

## Connections
* **Component of:** [[Parallel Efficiency]]
* **Limited By:** [[Amdahls Law]]
