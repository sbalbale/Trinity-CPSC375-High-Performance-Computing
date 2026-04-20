---
aliases: [Open Multi-Processing]
tags: 
  - #exam/tool
  - #CPSC375
sources: [Getting Started with OpenMP Programming.pdf, lec20.pdf]
created: 2026-04-20
updated: 2025-05-15
---

# OpenMP

> [!abstract] TL;DR Summary
> **OpenMP** (Open Multi-Processing) is the industry-standard API for **Shared Memory** parallel programming. It consists of compiler directives, runtime routines, and environment variables that implement the **Fork-Join Model**.

## Core Mechanics
* **Components:**
    1. **Directives:** `#pragma omp ...`
    2. **Library Functions:** `omp_get_thread_num()`, `omp_set_num_threads()`
    3. **Environment Variables:** `OMP_NUM_THREADS`

> [!code] Basic Parallel Region
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
