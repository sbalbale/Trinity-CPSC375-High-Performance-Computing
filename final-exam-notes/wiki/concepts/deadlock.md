---
aliases: [Circular Wait, MPI Deadlock]
tags: [#exam/theory, #hpc, #mpi]
sources: [lec20.txt, Homework 20.pdf, Homework 21.pdf, Homework 25.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Deadlock

> [!abstract]
> **Deadlock** is a state in which two or more threads or processes are unable to proceed because each is waiting for the other to release a resource or send a message. In parallel programming, it is a critical failure that causes the entire program to hang.

## Causes in HPC

### 1. Mismatched Point-to-Point
If Process A calls `MPI_Recv` from B, and Process B also calls `MPI_Recv` from A at the same time, both will wait forever.
> [!code] Deadlock Pattern
> ```c
> // P0
> MPI_Recv(..., 1, ...);
> MPI_Send(..., 1, ...);
> // P1
> MPI_Recv(..., 0, ...);
> MPI_Send(..., 0, ...);
> ```

### 2. Synchronization Barriers
If some processes reach an `MPI_Barrier` while others take a different branch of an `if` statement and never reach it, the processes at the barrier will wait indefinitely.

### 3. Circular Buffering
If a program relies on system buffering for large messages, it may deadlock once the internal buffer is exceeded (e.g., both processes `Send` large buffers before `Recv`).

## Prevention
- **Reorder Calls**: Ensure one process sends while the other receives.
- **Combined Calls**: Use `MPI_Sendrecv`.
- **Non-blocking Comm**: Use `MPI_Isend` and `MPI_Irecv`.

## Connections
* **Prerequisites:** [[shared-memory-model]], [[distributed-memory-model]].
* **Used In:** Analysis of [[odd-even-transposition-sort]] and [[cannons-algorithm]].
* **Contrasts With:** Livelock (where processes change state but make no progress).
