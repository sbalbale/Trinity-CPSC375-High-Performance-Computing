---
aliases: [omp_get_wtime, OpenMP Wall Clock]
tags: [openmp, performance, metrics]
sources: [More on OpenMP Programming.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Performance Measurement in OpenMP

> [!abstract]
> Performance measurement in OpenMP is done using the high-resolution timer **`omp_get_wtime()`**. It returns a `double` representing the number of seconds since some fixed point in the past.

## Core Mechanics

> [!equation] Elapsed Time
> $$T_{elapsed} = T_{finish} - T_{start}$$

### Calculating Parallel Metrics
Once the parallel execution time is measured, it can be compared to sequential time to determine scaling:
> [!code] Metric Logic
> ```c
> double speedup = serial_time / parallel_time;
> double efficiency = speedup / num_threads;
> 
> printf("Speedup: %f\n", speedup);
> printf("Efficiency: %f%%\n", efficiency * 100.0);
> ```

### Usage Guidelines
1. **Wall Clock**: `omp_get_wtime()` measures real (wall-clock) time, not CPU time.
2. **Team Synchronization**: In many cases, it is necessary to call `#pragma omp barrier` before and after the timed region to ensure all threads start and finish together.

> [!warning] Granularity
> While `omp_get_wtime()` is high-resolution, measuring very short blocks of code may be inaccurate due to the overhead of the function call itself or the operating system's scheduler.

## Implementations & Examples

> [!code] Standard Timing Pattern
> ```c
> double start = omp_get_wtime();
> #pragma omp parallel for
> for (int i = 0; i < N; i++) { /* work */ }
> double finish = omp_get_wtime();
> printf("Time: %f s\n", finish - start);
> ```

## Connections
* **Prerequisites:** [[openmp]].
* **Used In:** [[parallel-speedup]], [[parallel-efficiency]] calculation.
* **Contrasts With:** [[performance-measurement-mpi]] (`MPI_Wtime`).
