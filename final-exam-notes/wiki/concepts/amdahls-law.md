---
aliases: [Amdahl's Argument]
tags: [#exam/theory, #exam/formula]
sources: [lec17.pdf]
created: 2025-05-15
updated: 2025-05-15
---

# Amdahl's Law

> [!abstract] TL;DR Summary
> Predicts the maximum speedup of a program when only a fraction is parallelized. It proves that the **inherently sequential** part of a program sets a hard limit on performance gains.

## Core Mechanics

> [!equation] Amdahl's Formula
> $$S_p = \frac{1}{f + \frac{1-f}{p}}$$
> - $f$ = Fraction of the program that is **sequential** (non-parallelizable)
> - $1-f$ = Fraction that is parallelizable
> - $p$ = Number of processors

### Key Insight
As $p \to \infty$, the speedup $S_p$ approaches $1/f$.
Example: If 5% of your code is sequential ($f=0.05$), your maximum speedup is $1/0.05 = 20$, even if you have 1,000,000 processors.

> [!warning] Common Pitfalls
> - **Fixed Problem Size:** Amdahl's Law assumes the workload remains constant.
> - **Pessimism:** It can be overly pessimistic because, in reality, we often use more processors to solve **larger** problems, not just the same problem faster.

## Connections
* **Contrast With:** [[Gustafsons Law]]
* **Founded By:** [[Gene Amdahl]]
