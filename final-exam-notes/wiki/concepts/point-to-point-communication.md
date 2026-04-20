---
aliases: [MPI_Send, MPI_Recv, Direct Message Passing]
tags: [mpi, programming, communication]
sources: [lec20.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Point-to-Point Communication

> [!abstract]
> **Point-to-Point Communication** in MPI refers to messages sent between exactly two processes. It is the most basic form of data exchange in a distributed-memory system, implemented primarily through the `MPI_Send` and `MPI_Recv` functions.

## Core Mechanics

### MPI_Send
- Sends a buffer of data to a specific destination process.
- **Parameters**: Buffer, count, datatype, destination rank, tag, communicator.

### MPI_Recv
- Receives data from a specific source process.
- **Parameters**: Buffer, count, datatype, source rank, tag, communicator, status.
- **Blocking**: This function is **blocking**; it will not return until a matching message is received.

### Message Matching
For a message to be received, the `MPI_Recv` call must match the `MPI_Send` on three criteria:
1. **Communicator**: Same group of processes.
2. **Rank**: Receiver must specify sender's rank (or `MPI_ANY_SOURCE`).
3. **Tag**: User-defined integer ID for the message (or `MPI_ANY_TAG`).

> [!warning] Deadlock
> If two processes both call `MPI_Recv` from each other simultaneously, they will both wait indefinitely, causing a **deadlock**.

## Implementations & Examples

> [!code] Simple Exchange
> ```c
> if (rank == 0) {
>     MPI_Send(&val, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
> } else if (rank == 1) {
>     MPI_Recv(&val, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
> }
> ```

## Connections
* **Prerequisites:** [[mpi]], [[distributed-memory-model]].
* **Used In:** [[trapezoidal-rule-mpi]], [[parallel-mergesort]].
* **Contrasts With:** [[collective-communication]].
