---
aliases: [Scaled Speedup]
tags: [#exam/theory, #exam/formula]
sources: [lec17.pdf]
created: 2025-05-15
updated: 2025-05-15
---

# Gustafson's Law

> [!abstract] TL;DR Summary
> Proposes that speedup should be measured by scaling the problem size with the number of processors. It argues that **parallel speedup** can grow linearly if the workload is allowed to increase.

## Core Mechanics

> [!equation] Gustafson's Formula
> $$S_p = p + (1-p)f$$
> - $p$ = Number of processors
> - $f$ = Fraction of time spent on sequential parts in the **scaled** problem

### Key Insight
HPC users typically use more powerful machines to run **larger, more complex simulations**, rather than running a small problem in a fraction of a second. This "scaled speedup" is more representative of real-world supercomputing.

> [!warning] Common Pitfalls
> - **Assumes Scalable Workload:** Not all problems can be scaled indefinitely (e.g., physical memory limits or data dependencies).

## Connections
* **Contrast With:** [[Amdahls Law]]
* **Founded By:** [[John Gustafson]]
