---
aliases: [Message Passing Interface, MPI]
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

## Core Mechanics

> [!equation] MPI Model
> - **Distributed Memory:** Each process has its own private memory space.
> - **Communicators:** Groups of processes (e.g., `MPI_COMM_WORLD`).
> - **Rank:** A unique integer ID assigned to each process in a communicator (0 to $N-1$).
> - **Size:** The total number of processes in a communicator.

> [!warning] Common Pitfalls
> - **Deadlock:** Occurs when processes wait for messages that will never arrive (e.g., both calling `MPI_Recv`).
> - **Mismatched Arguments:** Collective calls (e.g., `MPI_Reduce`) must be called by all processes with compatible arguments.
> - **Blocking Calls:** `MPI_Recv` blocks execution until a message is received, which can cause delays.

## Implementations & Examples

> [!example] Basic Functions
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
