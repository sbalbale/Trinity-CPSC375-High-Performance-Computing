---
aliases: [Lecture 20 Summary, Introduction to MPI Summary]
tags: [#lecture/detailed, #CPSC375, #mpi]
sources: [lec20.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Lecture 20: Distributed-Memory Programming and MPI

> [!abstract]
> This lecture introduces distributed-memory systems and the **MPI** library. It covers program initialization, process identification, and basic point-to-point communication through the example of a parallel Trapezoidal Rule implementation.

## Distributed-Memory Model
- Each processor has its own **private memory**.
- To share data, processors must explicitly send and receive messages.
- Advantages: Scalability, management of memory hierarchy, and portability.

## MPI Program Structure
- **Header**: `#include <mpi.h>`
- **Initialization**: `MPI_Init(&argc, &argv)`
- **Identity**:
    - `MPI_Comm_size(MPI_COMM_WORLD, &size)`: Total number of processes.
    - `MPI_Comm_rank(MPI_COMM_WORLD, &rank)`: Unique ID for the current process ($0$ to $size-1$).
- **Cleanup**: `MPI_Finalize()`

## Point-to-Point Communication
- **MPI_Send**: Transmits a message to a destination rank.
- **MPI_Recv**: Blocks until a message is received from a source rank.
- **Message Matching**: Messages are matched based on the source rank, destination rank, and a user-defined **tag**.

## SPMD Execution
**Single Program, Multiple Data**. One program is written and compiled, but logical branches (usually based on `rank`) allow different processes to perform different tasks.
> [!example]
> If `rank == 0`, act as the coordinator (read input, gather results). Else, act as a worker (compute local part).

## Parallel Trapezoidal Rule
1. **Partition**: Divide the interval $[a, b]$ into $n$ trapezoids.
2. **Assign**: Each process computes the area for $n/p$ trapezoids.
3. **Communicate**: Worker processes send their local integrals to Process 0.
4. **Aggregate**: Process 0 receives all results and prints the final sum.
