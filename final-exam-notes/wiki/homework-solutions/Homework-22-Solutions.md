---
aliases: [Homework 22 Solutions]
tags: [#homework/solutions, #course_hpc, #mpi]
sources: [HW22_Solution.md, Homework 22.pdf]
created: 2026-04-20
updated: 2026-04-21
---

# Homework 22 Solutions

> [!abstract]
> Advanced MPI topics for Homework 22, including Derived Datatypes (Structs) and high-precision performance measurement.

## Question 1: MPI Derived Datatypes (Structs)

**Goal:** Create a type for `SimState { double vel; float mass; int coords[3]; }`.

### A. Creation
> [!code] Struct Creation
> ```c
> int blocklengths[3] = {1, 1, 3};
> MPI_Datatype types[3] = {MPI_DOUBLE, MPI_FLOAT, MPI_INT};
> MPI_Aint displacements[3];
> // ... compute displacements with MPI_Get_address ...
> MPI_Type_create_struct(3, blocklengths, displacements, types, &sim_type);
> MPI_Type_commit(&sim_type);
> ```

### B. Why `MPI_Get_address`?
**Padding**: C compilers insert padding bytes between fields for alignment. `sizeof()` calculations would miss these bytes, leading to **data corruption**. `MPI_Get_address` finds the actual runtime offset.

---

## Question 2: Performance Measurement

> [!code] Timing Template
> ```c
> MPI_Barrier(comm);
> start = MPI_Wtime();
> /* Computation */
> end = MPI_Wtime();
> local_elapsed = end - start;
> MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, comm);
> ```

### A. Purpose of `MPI_Barrier`
Ensures all processes start their timers at the **same wall-clock time**, preventing "startup skew" (scheduling delays) from polluting the computation measurement.

### B. Why `MPI_MAX`?
The total parallel runtime is determined by the **slowest process**. The application cannot proceed until all have finished.

### C. Removing the Barrier
Measurements become **unreliable and non-reproducible**. They would conflate OS jitter and scheduling delays with the actual algorithmic runtime.
