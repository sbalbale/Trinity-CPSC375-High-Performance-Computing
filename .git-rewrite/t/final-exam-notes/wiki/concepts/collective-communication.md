---
aliases: [MPI_Bcast, MPI_Reduce, MPI_Gather, MPI_Scatter, MPI_Allreduce, MPI_Allgather, Collective Communication]
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

## Key Characteristics
1. **Synchronous Nature**: All processes in the communicator must participate; there is an implicit synchronization barrier where processes wait for all others.
2. **Single Function Call**: A single MPI call initiates the coordination, with the underlying runtime handling the complex data movement.
3. **Optimized Patterns**: Implementations are highly tuned by the MPI library for the specific hardware and network.

## Rules for Usage
- **Rule 1: Global Participation**: Every process in the communicator **must** call the routine. You cannot match a collective call with a point-to-point call (e.g., `MPI_Reduce` cannot match `MPI_Recv`).
- **Rule 2: Compatible Arguments**: All processes must pass compatible arguments (e.g., the same `dest_proc` in `MPI_Reduce`, the same `MPI_Op`, and the same `datatype`).
- **Rule 3: Buffer Allocation**: While the output buffer (`out_data_p`) is only used by the destination process, all processes should provide a valid pointer (or `NULL` if the specific implementation allows it).
- **Rule 4: Order-Based Matching**: Unlike point-to-point, collectives do **not** use tags. Matching is strictly determined by the relative order of execution within the communicator.

## Tree-Structured Communication
For efficiency, MPI typically implements collectives using **tree structures** (like binary trees) instead of naive linear patterns.
- **Logarithmic Time**: Operations take $O(\log p)$ time instead of $O(p)$.
- **Parallel Movement**: Multiple processes can transmit data simultaneously, avoiding bottlenecks at the root process.
- **Scalability**: This allows collectives to function efficiently even with thousands of processes.

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
