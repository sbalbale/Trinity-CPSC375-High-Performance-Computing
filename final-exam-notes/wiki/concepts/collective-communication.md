---
aliases: [MPI_Bcast, MPI_Reduce, MPI_Gather, MPI_Scatter, MPI_Allreduce, MPI_Allgather]
tags: 
  - #exam/concept
  - #exam/mpi
sources: [lec21.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Collective Communication

> [!abstract] TL;DR Summary
> **Collective Communication** in MPI involves all processes within a specified communicator participating in a single communication operation, such as broadcasting, gathering, or reducing data.

## Core Mechanics

> [!warning] Common Pitfalls
> - **Tagless Matching:** Collective functions do not use tags; they match based on the order of execution.
> - **Deadlocks:** If not all processes in the communicator call the same collective function, the program will likely hang or crash.

## Implementations & Examples

> [!code] Implementation (If Applicable)
> ```c
> // Broadcast 'data' from process 0 to all other processes
> MPI_Bcast(&data, 1, MPI_INT, 0, MPI_COMM_WORLD);
> 
> // Gather 'local_data' from all processes into 'global_data' on process 0
> MPI_Gather(&local_data, 1, MPI_INT, &global_data, 1, MPI_INT, 0, MPI_COMM_WORLD);
> ```

## Connections
* **Prerequisites:** [[MPI]]
* **Contrasts With:** Point-to-Point communication (`MPI_Send`, `MPI_Recv`).
