---
aliases: [Synchronization Barrier, MPI_Barrier]
tags: [mpi, synchronization, collective]
sources: [lec22.txt]
created: 2026-04-20
updated: 2026-04-20
---

# MPI Barrier

> [!abstract]
> **MPI_Barrier** is a collective synchronization operation that blocks all processes in a communicator until **every process** has called it. It is primarily used to ensure a consistent state before starting a timed operation or moving to the next phase of an algorithm.

## Core Mechanics

> [!equation] Syntax
> `int MPI_Barrier(MPI_Comm comm);`

> [!warning] Common Pitfalls
> - **Performance Impact:** Excessive use of barriers can significantly degrade performance by forcing faster processes to wait for slower ones (**load imbalance**).
> - **Deadlock:** If one process in the communicator does not reach the barrier (e.g., due to an `if` branch), the entire program will **deadlock**.
> - **Over-synchronization:** Often, data dependencies (via `MPI_Send`/`MPI_Recv`) provide implicit synchronization, making explicit barriers redundant.

## Implementations & Examples

> [!example] Synchronized Timing
> Before measuring the execution time of a parallel block, a barrier is used to ensure all processes start at the same time:
> ```c
> MPI_Barrier(comm);
> start_time = MPI_Wtime();
> // parallel code
> finish_time = MPI_Wtime();
> ```

## Connections
* **Prerequisites:** [[mpi]], [[collective-communication]].
* **Used In:** [[performance-measurement-mpi]].
* **Contrasts With:** Implicit synchronization in point-to-point communication.
