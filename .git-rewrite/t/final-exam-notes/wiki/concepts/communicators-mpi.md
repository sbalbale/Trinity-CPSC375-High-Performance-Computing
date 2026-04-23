---
aliases: [MPI_Comm, MPI_COMM_WORLD]
tags: [mpi, programming, groups]
sources: [lec20.txt]
created: 2026-04-20
updated: 2026-04-20
---

# MPI Communicators

> [!abstract]
> An **MPI Communicator** is an object representing a collection of processes that can send messages to one another. Communicators provide a scope for communication, ensuring that messages sent within one library or module do not interfere with others.

## Core Mechanics

- **MPI_COMM_WORLD**: The default communicator created by `MPI_Init`. It includes all processes launched as part of the job.
- **Process ID (Rank)**: Every process in a communicator is assigned a unique integer ID from $0$ to $size-1$.
- **Sub-Communicators**: Processes can be divided into smaller groups (e.g., all processes in a single row of a mesh) using `MPI_Comm_split`.

### Core Functions
- `MPI_Comm_size`: Returns the total number of processes in the communicator.
- `MPI_Comm_rank`: Returns the rank of the calling process within the communicator.

> [!warning] Rank Relativity
> A process can belong to multiple communicators and may have a different **rank** in each one. Communication calls must always specify the communicator to which the destination rank belongs.

## Connections
* **Prerequisites:** [[mpi]].
* **Used In:** [[collective-communication]], [[cannons-algorithm]] (Row/Column communicators).
* **Contrasts With:** Shared memory thread IDs.
