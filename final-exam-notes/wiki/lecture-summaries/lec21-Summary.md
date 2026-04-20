---
tags: [#lecture/detailed, #exam/mpi]
sources: [lec21.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Collective Communication — Lecture Summary

> [!abstract] Summary
> A detailed overview of **MPI Collective Communication** operations. The lecture covers functions for broadcasting, reducing, scattering, and gathering data across all processes in a communicator, as well as strategies for data partitioning.

## I. Collective vs. Point-to-Point Communication
- **Matching Rules**: Collective communications are matched based on the communicator and the order in which they are called. They **do not use tags**.
- **Mandatory Participation**: All processes in the communicator must call the exact same collective function.
- **Compatibility**: Arguments passed by each process must be compatible. For example, the `dest_proc` must be identical across all calling processes.
- **Data Arguments**: The `out_data_p` argument is only used on the destination process (e.g., in `MPI_Reduce`), but all processes must pass a valid argument or `NULL`.

## II. Core Collective Operations
- **`MPI_Reduce`**: Combines data from all processes using a mathematical operator (`MPI_SUM`, `MPI_MAX`, etc.) and stores the final result on a single destination process.
- **`MPI_Allreduce`**: Performs a global reduction and distributes the final result back to all processes (equivalent to a Reduce followed by a Bcast). Useful when all processes need the global result to continue.
- **`MPI_Bcast` (Broadcast)**: Sends data belonging to a single root process to all other processes in the communicator.
- **`MPI_Scatter`**: Takes an array on the root process and distributes distinct, equal-sized chunks of it to each process.
- **`MPI_Gather`**: Takes local array chunks from each process and concatenates them into a single array on the root process.
- **`MPI_Allgather`**: Gathers data from all processes and distributes the fully concatenated result to every process.

## III. Data Partitioning
When splitting data arrays for parallel processing, three primary strategies are used:
1. **Block Partitioning**: Assigns contiguous blocks of components to each process.
2. **Cyclic Partitioning**: Assigns components in a round-robin fashion (e.g., process 0 gets elements 0, 3, 6; process 1 gets 1, 4, 7).
3. **Block-Cyclic Partitioning**: Uses a round-robin distribution of contiguous blocks.

## IV. Application Example: Matrix-Vector Multiplication
- Demonstrates combining collective operations to solve $y = Ax$.
- Uses `MPI_Allgather` to gather the distributed vector $x$ to all processes so that each process can compute its assigned rows of the result vector $y$.
