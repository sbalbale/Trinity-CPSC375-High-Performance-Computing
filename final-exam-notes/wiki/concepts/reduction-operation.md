---
aliases: [omp reduction]
tags: 
  - #exam/technique
  - #CPSC375
sources: [Getting Started with OpenMP Programming.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Reduction Operation

> [!abstract] TL;DR Summary
> An **OpenMP Reduction** operation (`reduction(op:var)`) efficiently combines private copies of a variable from multiple threads into a single final value using a specified operation (like `+`, `*`, `max`, or `min`) at the end of a parallel region.

## Core Mechanics

> [!warning] Common Pitfalls
> - **Operation Support:** Only specific arithmetic and logical operations are supported.

> [!code] Implementation (If Applicable)
> ```c
> int sum = 0;
> #pragma omp parallel for reduction(+:sum)
> for (int i=0; i<1000; i++) {
>     sum += 1;
> }
> ```

## Connections
* **Prerequisites:** [[OpenMP]], [[Race Condition]]
* **Contrasts With:** `#pragma omp critical` (reduction is generally much faster as it avoids locking per iteration).
