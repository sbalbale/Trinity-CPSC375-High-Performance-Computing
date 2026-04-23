---
aliases: [Single-Program Multiple-Data]
tags: 
  - #exam/theory
  - #course_hpc
sources: [lec20.txt, More on OpenMP Programming.txt]
created: 2026-04-20
updated: 2026-04-20
---

# SPMD

> [!abstract]
> **Single-Program Multiple-Data (SPMD)** is a parallel programming pattern where multiple processors execute the **same program** on different subsets of data. Control flow (e.g., `if (rank == 0)` or thread ID checks) allows processes to perform different tasks within the same codebase.

## Core Mechanics

- **Unified Source Code:** Only one executable is compiled and distributed to all nodes.
- **Process Differentiation:** Decisions are made based on each process's unique **rank** or **ID**.
- **Applicability**: While most commonly associated with **distributed-memory** (MPI), it is also used in **shared-memory** (OpenMP) parallel regions.

> [!warning] Common Pitfalls
> - **Precision Hazards**: In SPMD code where all processes perform the same floating-point comparison (e.g., to decide a branch), hardware differences can lead to different boolean results, causing processes to **diverge**.
> - **Complexity**: Extensive `if-else` branching based on rank can make the code difficult to follow (the "spaghetti" effect).

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

## Connections
* **Prerequisites:** [[mpi]], [[openmp]].
* **Used In:** [[parallel-mergesort]], [[sieve-of-eratosthenes]].
* **Contrasts With:** MIMD (Multiple-Instruction Multiple-Data).
