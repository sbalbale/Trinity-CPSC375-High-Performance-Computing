---
aliases: [private, shared, firstprivate, lastprivate]
tags: 
  - #exam/concept
  - #CPSC375
sources: [Getting Started with OpenMP Programming.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Data-Sharing Clauses

> [!abstract] TL;DR Summary
> **Data-Sharing Clauses** in OpenMP explicitly define the scope and initialization of variables inside a parallel region. They dictate whether a variable is shared among all threads or private to each.

## Core Mechanics

> [!warning] Common Pitfalls
> - **`private` Uninitialized:** Variables marked `private` are **uninitialized** at the start of the parallel region.
> - **`firstprivate` Initialization:** Each thread's private copy is initialized with the value of the original variable before entering the parallel region.
> - **`lastprivate` Write-back:** The value of the private copy from the *logically last* iteration of a loop is copied back to the original variable after the parallel region.
> - **`shared` default:** By default, variables declared outside the parallel region are `shared`.

## Implementations & Examples

> [!code] Implementation (If Applicable)
> ```c
> int x = 5;
> #pragma omp parallel firstprivate(x) lastprivate(x)
> {
>     // x starts at 5 in all threads
>     x = x + 1; 
>     // The 'x' from the logically last iteration is written back
> }
> ```

## Connections
* **Prerequisites:** [[OpenMP]]
