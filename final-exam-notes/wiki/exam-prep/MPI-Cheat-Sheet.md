---
aliases: [MPI Syntax, MPI Functions Sheet]
tags: [#exam/code, #prep/moc, #course_hpc]
sources: [lec20.md, lec21.md, lec22.md, Homework-20-Solutions.md, Homework-21-Solutions.md]
created: 2026-04-20
updated: 2026-04-20
---

# Cheat Sheet: MPI Reference

> [!abstract]
> Essential MPI function signatures, communication patterns, and matching rules.

## 1. Environment & Identity
> [!code]
> ```c
> MPI_Init(&argc, &argv);                 // Mandatory Start
> MPI_Comm_rank(MPI_COMM_WORLD, &rank);   // My ID (0 to p-1)
> MPI_Comm_size(MPI_COMM_WORLD, &size);   // Total Processes
> MPI_Finalize();                         // Mandatory End
> ```

## 2. Point-to-Point
> [!code]
> ```c
> // SENDER
> MPI_Send(buf, count, type, dest, tag, comm);
> // RECEIVER (BLOCKING)
> MPI_Recv(buf, count, type, src, tag, comm, &status);
> ```
> [!warning] Matching Rule
> Messages match only if **Communicator**, **Rank** (src), and **Tag** all match. `MPI_ANY_SOURCE` and `MPI_ANY_TAG` are wildcards.

## 3. Collective Communication
> [!code]
> ```c
> // 1-to-All
> MPI_Bcast(buf, count, type, root, comm);
> MPI_Scatter(send_buf, send_count, type, recv_buf, recv_count, type, root, comm);
> 
> // All-to-1
> MPI_Reduce(send_buf, recv_buf, count, type, op, root, comm);
> MPI_Gather(send_buf, send_count, type, recv_buf, recv_count, type, root, comm);
> 
> // All-to-All
> MPI_Allreduce(send_buf, recv_buf, count, type, op, comm);
> MPI_Allgather(send_buf, send_count, type, recv_buf, recv_count, type, comm);
> ```
> [!info] Valid Ops: `MPI_SUM`, `MPI_MAX`, `MPI_MIN`, `MPI_PROD`.

## 4. Advanced Primitives
> [!code]
> - **Barrier:** `MPI_Barrier(comm);` (Synchronization)
> - **Time:** `double t = MPI_Wtime();` (Seconds)
> - **Scan:** `MPI_Exscan(send_buf, recv_buf, count, type, op, comm);` (Exclusive Prefix Sum)
> - **Structs:** `MPI_Type_create_struct(...)` followed by `MPI_Type_commit(...)`.

## 5. Pitfalls
> [!warning]
> - **Deadlock:** Caused by circular wait in `MPI_Recv`.
> - **Buffer Mismatch:** Sending more data than the receiver's `count` allows.
> - **Root-only:** Result of `Reduce` or `Gather` is **only** valid at the root rank.
