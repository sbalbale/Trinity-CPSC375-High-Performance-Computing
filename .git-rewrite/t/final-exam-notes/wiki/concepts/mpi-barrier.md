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

### Common Use Cases
1. **Timing**: Ensuring all processes start and stop together for accurate wall-clock measurement.
2. **Debugging**: Forcing a specific execution order to isolate non-deterministic bugs or synchronization issues.
3. **Phase Coordination**: Ensuring all processes finish a computational phase (e.g., local sorting) before starting the next (e.g., merging).

### Performance Considerations
- **Latency**: Every barrier adds a fixed synchronization cost, which can be significant on high-latency networks.
- **Exposure of Imbalance**: The time spent in a barrier is determined by the **slowest process**. If one process is overloaded, all others remain idle, reducing overall efficiency.

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
