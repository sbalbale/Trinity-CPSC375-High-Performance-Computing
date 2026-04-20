---
aliases: [Efficiency]
tags: [#exam/formula, #hpc]
sources: [lec17.pdf]
created: 2025-05-15
updated: 2025-05-15
---

# Parallel Efficiency

> [!abstract] TL;DR Summary
> The fraction of time for which a processor is usefully employed. It measures the **utilization** of hardware resources in a parallel system.

## Core Mechanics

> [!equation] Efficiency Formula
> $$E_p = \frac{S_p}{p} = \frac{T_s}{p \cdot T_p}$$
> - $S_p$ = Speedup
> - $p$ = Number of processors
> - $T_s$ = Sequential time
> - $T_p$ = Parallel time

* **Range:** $0 \le E_p \le 1$.
* **Ideal:** $E_p = 1$ (Linear Speedup).

> [!warning] Common Pitfalls
> * **Decreasing Efficiency:** As $p$ increases for a fixed problem size, $E_p$ typically decreases due to increasing overhead relative to the shrinking work per processor.

## Connections
* **Calculated From:** [[Parallel Speedup]]
* **Used In:** [[Isoefficiency Function]]
