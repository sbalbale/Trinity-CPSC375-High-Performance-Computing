---
aliases: [Homework 22 Solutions]
tags: [#homework/solutions, #course_hpc, #mpi]
sources: [Homework 22.pdf, lec22.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Homework 22 Solutions

> [!abstract]
> Solutions to the advanced MPI exercises for Homework 22, covering derived datatype definition and accurate parallel timing methodologies.

## Problem 1: MPI Derived Datatypes

**Requirement:** Define a type for a state consisting of a `double velocity`, a `float mass`, and an `int coords[3]`.

### A. Defining the Type
> [!code] Implementation
> ```c
> MPI_Datatype state_type;
> int block_lengths[3] = {1, 1, 3};
> MPI_Datatype types[3] = {MPI_DOUBLE, MPI_FLOAT, MPI_INT};
> MPI_Aint displacements[3];
> 
> // Variables to find addresses
> double v; float m; int c[3];
> MPI_Aint base_address, addr_m, addr_c;
> 
> MPI_Get_address(&v, &base_address);
> MPI_Get_address(&m, &addr_m);
> MPI_Get_address(&c, &addr_c);
> 
> displacements[0] = 0;
> displacements[1] = addr_m - base_address;
> displacements[2] = addr_c - base_address;
> 
> MPI_Type_create_struct(3, block_lengths, displacements, types, &state_type);
> ```

### B. Why use `MPI_Get_address`?
You must use `MPI_Get_address` instead of `sizeof()` because compilers frequently insert **padding bytes** between structure members to satisfy hardware alignment requirements (e.g., a `double` must start at an address divisible by 8). If you simply add the sizes of the variables, the MPI displacement would not match the actual physical memory layout, leading to data corruption during communication.

### C. Commit and Free Sequence
> [!code] Lifecycle
> ```c
> // Before use
> MPI_Type_commit(&state_type);
> 
> // ... Use in MPI_Send / MPI_Recv ...
> 
> // When no longer needed
> MPI_Type_free(&state_type);
> ```

---

## Problem 2: Timing Analysis

### A. Purpose of `MPI_Barrier`
The specific purpose of calling `MPI_Barrier(comm)` before taking the start time is to ensure **synchronization**. It prevents any process from starting its local timer until **every** process in the communicator has reached the same point. This ensures the benchmark measures the execution of the parallel block rather than the time some processes spent waiting for others to arrive.

### B. Why `MPI_MAX`?
`MPI_MAX` is used because the wall-clock time of a parallel program is determined by the **slowest process** (the "bottleneck"). Using `MPI_SUM` would report an aggregate CPU time that doesn't reflect actual elapsed time, and `MPI_MIN` would only report the time of the fastest process, which would be an optimistic and incorrect representation of total runtime.

### C. Effect of removing the Barrier
If the barrier was removed, the reported elapsed time on the master node would become **unreliable**. 
- If Rank 0 reaches the start point later than others, it will report a shorter time than actual work.
- If Rank 0 reaches the start point much earlier, it might include its own idle time waiting for workers to finish in its measurement.
- The resulting data would reflect "synchronization skew" rather than the pure performance of the computation block.
