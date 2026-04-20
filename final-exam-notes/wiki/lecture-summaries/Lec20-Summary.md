---
aliases: [Intro to OpenMP and MPI Summary]
tags: [#lecture/detailed, #CPSC375, #openmp, #mpi]
sources: [lec20.pdf, lec20.txt]
created: 2025-05-15
updated: 2026-04-20
---

# Lecture 20 Summary: Introduction to OpenMP and MPI

> [!abstract] TL;DR
> Introduction to **OpenMP** (Shared Memory) and **MPI** (Distributed Memory). Explains the **Fork-Join Model** for threads and the **SPMD Pattern** for message-passing processes.

## 1. Parallel Architectures
* **Shared Memory:** Multiple cores share one global address space. Threads communicate via shared variables. (OpenMP)
* **Distributed Memory:** Each process has its own private memory. Communication requires explicit message passing over a network. (MPI)

## 2. OpenMP: The Fork-Join Model
1. **Master Thread:** Starts execution sequentially.
2. **Fork:** Master forks a team of **Worker Threads** for parallel regions.
3. **Join:** Threads synchronize and terminate/park at the end of the region.

### Basic Directives
* `#pragma omp parallel`: Defines a parallel region.
* `omp_get_thread_num()`: Returns thread ID (Master is 0).

## 3. MPI: The SPMD Model
* **Definition:** **Single-Program Multiple-Data**. One program is written and executed by $N$ processes.
* **Rank:** Unique ID for each process ($0$ to $N-1$).
* **Communicator:** A collection of processes (e.g., `MPI_COMM_WORLD`).

### Basic MPI Functions
* `MPI_Init`: Setup environment.
* `MPI_Finalize`: Cleanup.
* `MPI_Comm_size`: Total process count.
* `MPI_Comm_rank`: Process's unique ID.
* `MPI_Send` / `MPI_Recv`: Send and receive messages.

## 4. Example: Trapezoidal Rule in MPI
* The problem is partitioned into tasks (calculating local integrals).
* Processes calculate their local results and send them to **Process 0** (the Master).
* Process 0 receives all local integrals and sums them for the final result.

## 5. Input/Output in MPI
* **Stdout:** Most implementations allow all processes to print, though order is unpredictable.
* **Stdin:** Usually only **Process 0** has access to stdin. It must read data and then `MPI_Send` it to other processes.
