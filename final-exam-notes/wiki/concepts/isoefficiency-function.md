---
aliases: [Isoefficiency]
tags: [#exam/theory, #hpc]
sources: [lec17.pdf]
created: 2025-05-15
updated: 2025-05-15
---

# Isoefficiency Function

> [!abstract] TL;DR Summary
> Relates problem size ($W$) to the number of processors ($p$) required to maintain a **constant efficiency**. It is the primary metric for evaluating the **scalability** of a parallel system.

## Core Mechanics
* **Condition:** Efficiency $E$ is held constant.
* **Function:** $W = f(p)$.
* **Highly Scalable:** $f(p)$ is linear or near-linear (e.g., $O(p)$).
* **Poorly Scalable:** $f(p)$ grows rapidly (e.g., $O(p^2)$ or $O(2^p)$).

### Calculation
Derived from the overhead equation $T_o = p T_p - T_s$.
To keep $E$ constant: $W = \frac{E}{1-E} T_o$.

> [!warning] Common Pitfalls
> * **Hardware Dependency:** The isoefficiency function depends on both the algorithm and the underlying hardware architecture (network speed, latency).

## Connections
* **Metric For:** [[Scalability]]
* **Depends On:** [[Overhead (Parallel Computing)]]
