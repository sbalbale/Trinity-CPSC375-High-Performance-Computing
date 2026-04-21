---
aliases: [Message Passing Interface, MPI, mpi-hello-world]
tags: 
  - #exam/entity
  - #course_hpc
sources: [lec20.txt, lec21.txt, lec22.txt]
created: 2026-04-20
updated: 2026-04-20
---

# MPI

> [!abstract] TL;DR Summary
> The **Message Passing Interface (MPI)** is a standardized library specification for **distributed-memory** parallel programming. It allows processes on different machines to communicate via explicit **send and receive** operations.
## Advantages of the MPI Model
- **Explicit Memory Management**: Gives programmers control over data placement and movement, leading to better cache optimization.
- **Portability**: Code runs on clusters, supercomputers, and cloud platforms without modification.
- **Determinism**: Explicit data transfers make behavior more reproducible than shared-memory models.
- **Simplified Debugging**: Traceable message flow makes deadlocks and performance bottlenecks easier to isolate.

## Core Mechanics

### Process Properties (SPMD)
MPI programs typically follow the **Single Program, Multiple Data (SPMD)** model:
1. **Fixed Number**: The process count is specified at startup and remains constant.
2. **Same Program**: Every process executes the exact same binary.
3. **Unique ID (Rank)**: Logical branching (e.g., `if (rank == 0)`) allows processes to perform different tasks.
4. **Lifecycle**: Processes alternate between **Computation Phases** and **Communication Phases**.

### Historical Evolution
- **Late 1980s**: Proprietary vendor-specific libraries (not portable).
- **1989**: PVM (Parallel Virtual Machine) - first standard attempt.
- **1994 (MPI 1.0)**: Official portable standard released.
- **1997 (MPI 2.0)**: Added dynamic processes and I/O.
- **2021 (MPI 4.0)**: Current standard with modernization for exascale systems.

## Implementations & Examples

### Naming Conventions
- All identifiers begin with `MPI_`.
- First letter after the underscore is uppercase (e.g., `MPI_Send`).
- Helps distinguish library calls from application code.

> [!example] Basic Functions
...
> - `MPI_Init`: Initializes the environment.
> - `MPI_Finalize`: Cleans up the environment.
> - `MPI_Comm_rank`: Gets the process rank.
> - `MPI_Comm_size`: Gets the total number of processes.
> - `MPI_Send` / `MPI_Recv`: Point-to-point communication.

> [!example] Advanced Functions
> - `MPI_Bcast` / `MPI_Reduce`: [[collective-communication]] operations.
> - `MPI_Type_create_struct`: Creating [[mpi-derived-datatypes]].
> - `MPI_Barrier`: Synchronization [[mpi-barrier]].
> - `MPI_Wtime`: [[performance-measurement-mpi]].

> [!code] MPI "Hello World"
> ```c
> #include <mpi.h>
> int main() {
>     int rank, size;
>     MPI_Init(NULL, NULL);
>     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
>     MPI_Comm_size(MPI_COMM_WORLD, &size);
>     printf("Greetings from process %d of %d\n", rank, size);
>     MPI_Finalize();
> }
> ```

## Connections
* **Prerequisites:** [[distributed-memory-model]]
* **Used In:** [[collective-communication]], [[trapezoidal-rule-mpi]]
* **Contrasts With:** [[openmp]]
