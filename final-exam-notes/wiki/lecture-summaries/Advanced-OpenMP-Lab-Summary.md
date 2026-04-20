---
aliases: [Advanced OpenMP Lab Summary, Advanced Coordination in OpenMP]
tags: [#lecture/detailed, #CPSC375, #openmp]
sources: [More on OpenMP Programming.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Lab: More on OpenMP Programming

> [!abstract]
> This lab explores advanced thread coordination, dynamic scheduling, task-based parallelism, and performance profiling in **OpenMP**.

## Advanced Synchronization
- **Master**: `#pragma omp master`. Only thread 0 executes. No implicit barrier.
- **Single**: `#pragma omp single`. Any thread executes once. Implicit barrier follows (unless `nowait`).
- **Barrier**: `#pragma omp barrier`. Explicit synchronization for the entire team.
- **Atomic**: `#pragma omp atomic`. Lightweight protection for a single memory update (e.g., `x++`). Faster than `critical`.

## Performance Optimizations
### The `nowait` Clause
Used with `for` or `single` to remove the implicit barrier at the end of the block.
> [!warning]
> Only use `nowait` when there are no data dependencies on the loop/block results in the following code.

### Loop Scheduling
`#pragma omp parallel for schedule(type, chunk)`
- **Static**: Assigns iterations in fixed blocks. Minimal overhead.
- **Dynamic**: Assigns iterations at runtime. Better for irregular work where some iterations take longer than others.

## Tasks and Sections
### Sections
Concurrently execute independent blocks of code.
> [!code] Sections Syntax
> ```c
> #pragma omp parallel sections
> {
>     #pragma omp section
>     { work_A(); }
>     #pragma omp section
>     { work_B(); }
> }
> ```

### Tasks
Independent units of work that can be executed by any available thread.
- Typically generated inside a `single` block using `#pragma omp task`.

## Performance Measurement
`omp_get_wtime()` returns the elapsed wall-clock time in seconds.
> [!code] Timing Block
> ```c
> double start = omp_get_wtime();
> // parallel code
> double end = omp_get_wtime();
> printf("Elapsed: %f s\n", end - start);
> ```
