---
aliases: [Scaled Speedup, Weak Scaling Law]
tags: [#exam/theory, #exam/formula, performance]
sources: [lec17.pdf, lec27.txt]
created: 2025-05-15
updated: 2026-04-20
---

# Gustafson's Law

> [!abstract] TL;DR Summary
> Proposes that speedup should be measured by scaling the problem size with the number of processors. It argues that **parallel speedup** can grow linearly if the workload increases proportionally, making it the theoretical foundation for **weak-scalability**.

## Core Mechanics

> [!equation] Gustafson's Formula
> $$S_p = p + (1-p)f$$
> - $p$ = Number of processors
> - $f$ = Fraction of time spent on sequential parts in the **scaled** problem

### Key Insight
HPC users typically use more powerful machines to run **larger, more complex simulations** in the same amount of time, rather than running a fixed problem in a fraction of a second. This "scaled speedup" better reflects real-world supercomputing.

> [!warning] Common Pitfalls
> - **Assumes Scalable Workload:** Not all problems can be scaled indefinitely (e.g., physical memory limits or communication overhead).

## Connections
* **Used In:** [[weak-scalability]].
* **Contrasts With:** [[amdahls-law]].
