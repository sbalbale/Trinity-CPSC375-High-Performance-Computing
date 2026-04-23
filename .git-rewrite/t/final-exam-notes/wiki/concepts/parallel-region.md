---
aliases: [omp parallel, SPMD region, Parallel Region]
tags: 
  - #exam/concept
  - #CPSC375
sources: [Getting Started with OpenMP Programming.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Parallel Region

> [!abstract]
> A **Parallel Region** is a block of code in an OpenMP program that is executed by multiple threads simultaneously. It is the fundamental building block of the [[fork-join-model]], where a single master thread forks into a team of worker threads.

## Core Mechanics

### Lifecycle
1. **Fork**: The master thread encounters the `#pragma omp parallel` directive and creates a team of threads.
2. **Execute**: All threads in the team execute the code within the curly braces `{}`.
3. **Join**: At the end of the block, there is an implicit **barrier**. All threads wait for the team to finish before the master thread continues alone.

> [!warning] Common Pitfalls
> - **Redundant Work**: Without additional directives (like `omp for`), every thread executes the **entire** block. If the block contains a `printf`, you will see $N$ print statements.
> - **Implicit Barrier**: The master thread cannot proceed until **all** threads in the region have completed.

## Implementations & Examples

> [!code] Example
> ```c
> omp_set_num_threads(4);
> #pragma omp parallel
> {
>     int id = omp_get_thread_num();
>     printf("Hello from thread %d\n", id);
> }
> ```

## Connections
* **Prerequisites:** [[openmp]], [[fork-join-model]].
* **Used In:** [[parallel-loop-openmp]], [[reduction-operation]].
* **Contrasts With:** Serial execution.
