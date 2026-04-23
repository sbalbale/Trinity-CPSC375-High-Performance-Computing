---
aliases: [omp task, omp section, Task Parallelism]
tags: [openmp, concurrency, programming]
sources: [More on OpenMP Programming.txt]
created: 2026-04-20
updated: 2026-04-20
---

# OpenMP Tasks and Sections

> [!abstract]
> **OpenMP Tasks and Sections** provide mechanisms for parallelizing code that does not fit the simple "loop" model. Sections allow for concurrent execution of independent blocks, while tasks allow for **dynamic scheduling** of independent units of work.

## Core Mechanics

### Parallel Sections
Used to execute different, non-iterative pieces of work concurrently.
- **Syntax**: `#pragma omp parallel sections` with inner `#pragma omp section` blocks.
- **Behavior**: Each section is assigned to an available thread.

### Parallel Tasks
Used for parallelizing irregular or recursive structures (like traversing a tree or linked list).
- **Syntax**: `#pragma omp task`.
- **Generation**: Typically generated inside a `single` or `master` block so that tasks are not duplicated by every thread in the region.

> [!warning] Task Execution
> When a thread encounters a `task` pragma, it may execute the task immediately or defer it for execution by any thread in the team. There is no guarantee of order.

## Implementations & Examples

> [!code] Task Generation Pattern
> ```c
> #pragma omp parallel
> {
>     #pragma omp single
>     {
>         for (node = head; node; node = node->next) {
>             #pragma omp task
>             process(node);
>         }
>     }
> }
> ```
One thread generates all tasks; the entire team of threads executes them.

## Connections
* **Prerequisites:** [[openmp]], [[omp-synchronization]].
* **Used In:** Recursive algorithms, linked-list processing.
* **Contrasts With:** [[parallel-loop-openmp]].
