---
aliases: [C&E, Comparison Swap]
tags: [algorithm, sorting, mpi]
sources: [lec25.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Compare-and-Exchange

> [!abstract]
> **Compare-and-Exchange** is the fundamental primitive of most parallel sorting algorithms. Two elements $A$ and $B$ are compared, and their positions are swapped if they are out of their final sorted order. In parallel systems, this involves **message passing** between processors.

## Core Mechanics

### Comparison Patterns
- **Sequential Exchange**: $P_1$ sends $A$, $P_2$ compares and sends back the result ($O(3)$ steps).
- **Simultaneous Exchange**: Both processes send and receive concurrently ($O(2)$ steps).
    - **Pros**: Better parallelism, simpler logic (both processes do similar work), and more elegant symmetry.

## Reality Check: Communication vs. Computation
In parallel sorting, the fundamental bottleneck is often the massive disparity between computation and communication speeds:
- **Computation**: A single comparison is $O(1)$ and takes nanoseconds.
- **Communication**: A single message takes microseconds (1000x slower).
**Result**: Message overhead typically dominates the execution time. This is mitigated by **Hybrid Approaches** (local sequential sorting followed by minimal parallel communication) or **Batching** multiple comparisons into a single message.

> [!warning] Precision Hazard
> When using Method 2, if $A$ and $B$ are floating-point numbers, different processors might arrive at different boolean results for the comparison `A > B` due to hardware precision differences. This can break **SPMD consistency**.

## Implementations & Examples

> [!code] Dual Comparison MPI
> ```c
> // Process P1 (Lower Rank)
> MPI_Send(&A, 1, MPI_DOUBLE, P2, 0, comm);
> MPI_Recv(&B, 1, MPI_DOUBLE, P2, 0, comm, &status);
> if (A > B) A = B; // P1 keeps smaller
> 
> // Process P2 (Higher Rank)
> MPI_Recv(&A, 1, MPI_DOUBLE, P1, 0, comm, &status);
> MPI_Send(&B, 1, MPI_DOUBLE, P1, 0, comm);
> if (A > B) B = A; // P2 keeps larger
> ```

## Connections
* **Prerequisites:** [[mpi]], [[parallel-sorting-basics]].
* **Used In:** [[odd-even-transposition-sort]], [[bitonic-sort]].
* **Contrasts With:** [[merge-and-split]] (scaling for $n > p$).
