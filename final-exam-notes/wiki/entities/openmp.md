---
aliases: [Open Multi-Processing]
tags: 
  - #exam/tool
  - #CPSC375
sources: [Getting Started with OpenMP Programming.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# OpenMP

> [!abstract] TL;DR Summary
> **OpenMP** (Open Multi-Processing) is an API that supports multi-platform shared-memory multiprocessing programming in C, C++, and Fortran. It uses compiler directives (pragmas), library routines, and environment variables to define parallel execution.

## Core Mechanics

> [!code] Implementation (If Applicable)
> ```c
> #include <omp.h>
> 
> int main() {
>     #pragma omp parallel
>     {
>         int id = omp_get_thread_num();
>     }
> }
> ```

## Connections
* **Used In:** [[Parallel Region]], [[Reduction Operation]], [[Data-Sharing Clauses]]
* **Contrasts With:** [[MPI]]
