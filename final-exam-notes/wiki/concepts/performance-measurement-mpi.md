---
aliases: [MPI Profiling, MPI_Wtime]
tags: [mpi, performance, exam/formula]
sources: [lec22.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Performance Measurement in MPI

> [!abstract]
> Performance measurement in MPI involves determining the **wall-clock time** taken by parallel sections of code. The primary tool is `MPI_Wtime()`, and accurate measurement requires **synchronization** across all processes to find the maximum elapsed time.

## Core Mechanics

> [!equation] Elapsed Time
> $$T_{total} = \max(T_{local\_finish} - T_{local\_start})$$
> - $T_{local\_start}$: Start time on a specific process.
> - $T_{local\_finish}$: End time on a specific process.
> - $T_{total}$: The time taken by the slowest process, representing the total parallel time.

> [!warning] Common Pitfalls
> - **Lack of Synchronization:** Without an `MPI_Barrier` before the start time is recorded, processes might start measuring at different points, leading to inaccurate results.
> - **Ignoring Load Imbalance:** Simply taking the average time is misleading; the **maximum** time across all processes determines the overall program duration.

## Implementations & Examples

> [!code] Timing Block
> ```c
> double local_start, local_finish, local_elapsed, total_elapsed;
> MPI_Barrier(comm);
> local_start = MPI_Wtime();
> /* Parallel Code Block */
> local_finish = MPI_Wtime();
> local_elapsed = local_finish - local_start;
> MPI_Reduce(&local_elapsed, &total_elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, comm);
> if (my_rank == 0) printf("Total time: %f s\n", total_elapsed);
> ```

## Connections
* **Prerequisites:** [[mpi-barrier]], [[reduction-operation]].
* **Used In:** [[parallel-speedup]], [[parallel-efficiency]].
* **Contrasts With:** Serial timing using `time.h` or `gettimeofday`.
