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

### MPI Message-Passing Methods
- **Method 1 (Single Comparison)**: Process $P_1$ sends $A$ to $P_2$. $P_2$ compares $A$ and $B$ and sends the correct value back to $P_1$. (3 steps)
- **Method 2 (Dual Comparison)**: Both processes send their values to each other simultaneously. Both perform the comparison locally to determine which value to keep. (2 steps)

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
