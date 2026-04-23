---
aliases: [Amdahl's Argument, Fixed-Size Scaling]
tags: [#exam/theory, #exam/formula, performance]
sources: [lec17.pdf, lec27.txt]
created: 2025-05-15
updated: 2026-04-20
---

# Amdahl's Law

> [!abstract] TL;DR Summary
> Predicts the maximum speedup of a program for a **fixed problem size** when only a fraction is parallelized. It proves that the **inherently sequential** part of a program sets a hard ceiling on performance gains.

## Core Mechanics

> [!equation] Amdahl's Formula (Ideal)
> $$S_p = \frac{1}{f + \frac{1-f}{p}}$$
> - $f$: Sequential fraction.
> - $p$: Number of processors.

> [!equation] Revised Amdahl's Law (Realistic)
> In practice, communication and synchronization add overhead that increases with $p$.
> $$S_p = \frac{1}{f + \frac{1-f}{p} + \text{overhead}(p)}$$

### Key Insight
The **serial fraction is the fundamental limit** on speedup for fixed-size problems. Even with 1000s of processors, a 10% serial fraction limits you to 10x speedup.

| $p$ (Processors) | $S_p$ (90% parallelizable) | $S_p$ (95% parallelizable) |
| :--- | :--- | :--- |
| 1 | 1.0 | 1.0 |
| 2 | 1.82 | 1.90 |
| 16 | 6.40 | 8.89 |
| 100 | 9.17 | 16.81 |
| $\infty$ | **10.0** | **20.0** |

> [!warning] Common Pitfalls
> - **Fixed Problem Size:** Amdahl's Law assumes the workload remains constant, making it the theoretical model for **strong-scalability**.
> - **Pessimism:** It can be overly pessimistic because, in reality, we often use more processors to solve **larger** problems.

## Connections
* **Used In:** [[strong-scalability]].
* **Diagnosed By:** [[karp-flatt-metric]] (when $e$ remains constant).
* **Contrasts With:** [[gustafsons-law]].
