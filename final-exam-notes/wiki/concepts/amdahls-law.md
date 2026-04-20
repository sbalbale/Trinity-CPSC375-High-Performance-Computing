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

> [!equation] Amdahl's Formula
> $$S_p = \frac{1}{f + \frac{1-f}{p}}$$
> - $f$ = Fraction of the program that is **sequential** (non-parallelizable)
> - $1-f$ = Fraction that is parallelizable
> - $p$ = Number of processors

### Key Insight
As $p \to \infty$, the speedup $S_p$ approaches $1/f$.
Example: If 10% of your code is sequential ($f=0.1$), your maximum speedup is $1/0.1 = 10$, even with infinite processors.

> [!warning] Common Pitfalls
> - **Fixed Problem Size:** Amdahl's Law assumes the workload remains constant, making it the theoretical model for **strong-scalability**.
> - **Pessimism:** It can be overly pessimistic because, in reality, we often use more processors to solve **larger** problems.

## Connections
* **Used In:** [[strong-scalability]].
* **Diagnosed By:** [[karp-flatt-metric]] (when $e$ remains constant).
* **Contrasts With:** [[gustafsons-law]].
