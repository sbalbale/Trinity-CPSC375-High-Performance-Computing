---
aliases: [Single-Program Multiple-Data]
tags: 
  - #exam/theory
  - #course_hpc
sources: [lec20.txt]
created: 2026-04-20
updated: 2026-04-20
---

# SPMD

> [!abstract] TL;DR Summary
> **Single-Program Multiple-Data (SPMD)** is a parallel programming pattern where multiple processors execute the **same program** on different subsets of data. Control flow (e.g., `if (rank == 0)`) allows processes to perform different tasks.

## Core Mechanics

> [!equation] Operation Pattern
> - **Unified Source Code:** Only one executable is compiled and distributed to all nodes.
> - **Process Differentiation:** Decisions are made based on each process's unique **rank** or **ID**.
> - **Coordination:** Processes coordinate via communication (e.g., `MPI_Send`, `MPI_Recv`).

> [!warning] Common Pitfalls
> - **Implicit Branching:** Complex `if-else` structures based on ranks can lead to code that is difficult to maintain or debug.
> - **Synchronization:** Ensuring all processes reach a certain point in the logic before continuing requires careful use of barriers or collective calls.
> - **Data Distribution:** Programmers are responsible for partitioning data so that each process works on its unique piece.

## Implementations & Examples

> [!example] Master-Worker Logic
> In an SPMD program, rank 0 is often designated as the master to handle I/O or coordination.
> ```c
> if (rank == 0) {
>     // Master logic (e.g., read input)
> } else {
>     // Worker logic (e.g., perform computation)
> }
> ```

> [!code] MPI SPMD Example
> ```c
> MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
> if (my_rank != 0) {
>     MPI_Send(data, count, MPI_INT, 0, 0, MPI_COMM_WORLD);
> } else {
>     for (int q = 1; q < size; q++) {
>         MPI_Recv(data, count, MPI_INT, q, 0, MPI_COMM_WORLD, &status);
>     }
> }
> ```

## Connections
* **Prerequisites:** [[MPI]]
* **Used In:** [[Parallel Algorithm Design]]
* **Contrasts With:** [[MISD]], [[MIMD]]
