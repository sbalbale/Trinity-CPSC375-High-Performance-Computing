---
aliases: [Efficiency]
tags: [#exam/formula, performance]
sources: [lec17.pdf, lec27.txt]
created: 2025-05-15
updated: 2026-04-20
---

# Parallel Efficiency

> [!abstract] TL;DR Summary
> The fraction of time for which a processor is usefully employed. It measures the **utilization** of hardware resources in a parallel system. Ideal efficiency is $E=1$.

## Core Mechanics

> [!equation] Efficiency Formula
> $$E_p = \frac{S_p}{p} = \frac{T_s}{p \cdot T_p}$$
> - $S_p$ = Speedup
> - $p$ = Number of processors
> - $T_s$ = Sequential time
> - $T_p$ = Parallel time

* **Range:** $0 \le E_p \le 1$.
* **Interpretation**: $E = 0.5$ means processors are doing useful work only half of the time.

> [!warning] Scaling Trends
> - In **strong-scalability**, $E$ decreases as $p$ increases because overhead begins to dominate the shrinking local workload.
> - In **weak-scalability**, the goal is to keep $E$ constant as both $p$ and the problem size grow.

## Connections
* **Calculated From:** [[parallel-speedup]].
* **Used In:** [[isoefficiency-function]], [[strong-scalability]], [[weak-scalability]].
