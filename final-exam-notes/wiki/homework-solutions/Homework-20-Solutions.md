---
aliases: [Homework 20 Solutions]
tags: [#homework/solutions, #course_hpc, #mpi]
sources: [HW20_Solution.md, Homework 20.pdf]
created: 2026-04-20
updated: 2026-04-21
---

# Homework 20 Solutions

> [!abstract]
> Introduction to MPI and Distributed Memory Programming for Homework 20, covering architecture models, SPMD, and point-to-point communication.

## Problem 1: Shared vs. Distributed Memory

| Feature | Shared Memory | Distributed Memory |
|---|---|---|
| **Memory Access** | Direct (unified address space) | Indirect (private address spaces) |
| **Communication** | Implicit (shared variables) | Explicit (message passing) |
| **Advantage** | Easier programming | **Highly scalable** |
| **Disadvantage** | Limited scalability (bus bottleneck) | Complex data management |

---

## Problem 2: MPI Process Characteristics

- **Fixed Count**: Set at launch time (`mpiexec -n N`) and remains constant.
- **SPMD Model**: Every process runs the **same executable** but uses its **unique rank** (0 to $N-1$) to operate on different data.
- **Independence**: Processes have private memory; no direct access to peer memory.

---

## Problem 3: Lifecycle Management

- **`MPI_Init`**: Sets up the communication infrastructure, assigns ranks, and defines `MPI_COMM_WORLD`.
- **`MPI_Finalize`**: Cleans up resources and tears down network connections.
> [!danger] Undefined Behavior
> Calling MPI functions before `Init` or after `Finalize` typically results in **segfaults or deadlocks** as the environment is uninitialized or destroyed.

---

## Problem 4: Communicators and Rank/Size

- **Communicator**: A named group of processes (e.g., `MPI_COMM_WORLD`). Enforces message isolation.
- **`MPI_Comm_size`**: Returns total process count ($p$). Used for dividing total work.
- **`MPI_Comm_rank`**: Returns unique ID (rank). Used for task differentiation (e.g., `if (rank == 0)`).

---

## Problem 5: `MPI_Send` and `MPI_Recv`

> [!equation] Message Matching
> A receive matches a send if and only if:
> 1. **Communicator** matches.
> 2. **Source/Destination** ranks match.
> 3. **Tag** matches.

- **Blocking**: `MPI_Recv` blocks until a message is received. `MPI_Send` behavior is implementation-dependent (buffered for small messages, synchronous/blocking for large ones).

---

## Problem 6: Wildcards and Status

- **Wildcards**: `MPI_ANY_SOURCE` and `MPI_ANY_TAG` allow receiving from any peer.
- **`MPI_Status`**: A struct containing the actual `MPI_SOURCE` and `MPI_TAG` after a wildcard receive.
- **`MPI_Get_count`**: Queries the status to find the **actual number of elements** received.

---

## Problem 7: Issues and Deadlock

> [!warning] Deadlock Risk
> Symmetric blocking sends (e.g., two processes sending to each other before receiving) can cause **deadlock** if the messages are too large to be buffered.
> **Solution**: Alternate send/receive order or use non-blocking `MPI_Isend`/`MPI_Irecv`.
