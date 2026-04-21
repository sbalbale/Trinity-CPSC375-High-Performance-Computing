---
aliases: [omp private, omp shared, variable scoping]
tags: 
  - #exam/concept
  - #CPSC375
sources: [Getting Started with OpenMP Programming.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Data-Sharing Clauses

> [!abstract]
> **Data-Sharing Clauses** in OpenMP explicitly define the scope and lifecycle of variables inside a parallel region. They are essential for preventing [[race-condition]]s and ensuring that each thread has the data it needs to perform its task.

## Core Mechanics

| Clause | Visibility | Initialized? | Final Value to Original? |
| :--- | :--- | :--- | :--- |
| `shared(x)` | All threads | Yes (from original) | **Yes** (modifies original) |
| `private(x)` | Local copy | **No** (garbage) | **No** (original unchanged) |
| `firstprivate(x)`| Local copy | **Yes** (from original) | **No** (original unchanged) |
| `lastprivate(x)` | Local copy | **No** (garbage) | **Yes** (from logical last) |

> [!warning] Common Pitfalls
> - **Uninitialized Private:** A common bug is using a `private` variable without initializing it within the parallel region. Use `firstprivate` if you need the initial value.
> - **Shared Performance:** Accessing `shared` variables can lead to **false sharing** or bus contention if multiple threads write to them frequently.
> - **Combining Clauses**: You can use `firstprivate(x) lastprivate(x)` on the same variable to both initialize it from the master and copy its final value back.

## Implementations & Examples

### Combining Clauses
> [!code] Example
> ```c
> int x = 10, y = 0;
> #pragma omp parallel for firstprivate(x) lastprivate(y)
> for (int i = 0; i < 100; i++) {
>    // x is 10 in every thread's local scope
>    y = x + i; 
> }
> // y now holds 10 + 99 = 109
> ```

## Connections
* **Prerequisites:** [[openmp]], [[parallel-region]].
* **Used In:** [[parallel-loop-openmp]], avoiding [[race-condition]].
