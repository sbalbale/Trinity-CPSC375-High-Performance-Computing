---
aliases: [Exam 2 MOC, HPC Exam 2 Study Guide]
tags: [#prep/moc, #exam/theory, #course_hpc]
sources: [lec15.md, lec16.md, lec17.md, lec20.md, lec21.md, lec22.md, lec23.md, lec24.md, lec25.md, lec27.md, lec28.md, lec29.md, lec30.md, lec31.md]
created: 2026-04-20
updated: 2026-04-20
---

# Exam 2 Map of Content (Lectures 15-31)

> [!abstract]
> This MOC provides a high-density retrieval system for the concepts covered in Lectures 15-31, focusing on **Parallel Algorithm Design**, **Performance Analysis**, **MPI/OpenMP**, **Sorting**, and **Matrix Multiplication**.

## 1. Parallel Algorithm Design (Foster's Methodology)
> [!info] Core Framework: [[Foster's Methodology]] (PCAM)
> 1. **Partitioning**: Focus on exposing maximum parallelism. [[domain-decomposition]] vs [[functional-decomposition]].
> 2. **Communication**: Local vs Global. Focus on minimizing overhead.
> 3. **Agglomeration**: Grouping tasks to reduce communication and management costs.
> 4. **Mapping**: Assigning tasks to processors. [[mapping-parallel-programming]].

> [!warning] Mapping Pitfalls
> - **Static Mapping**: Use for predictable loads. [[static-mapping]].
> - **Dynamic Mapping**: Use for irregular loads (e.g., [[Mandelbrot Set Generation]]). [[master-worker-pattern]].

## 2. Performance Analysis
> [!equation] Fundamental Formulas
> - **Speedup**: $S_p = \frac{T_s}{T_p}$
> - **Efficiency**: $E = \frac{S_p}{p}$
> - **Amdahl's Law (Strong Scaling)**: $S_p = \frac{1}{f + (1-f)/p}$ (Limit: $1/f$)
> - **Gustafson's Law (Weak Scaling)**: $S_p = p + (1-p)f$
> - **Karp-Flatt Metric**: $e = \frac{1/S_p - 1/p}{1 - 1/p}$ (Diagnoses overhead vs serial fraction)
> - **Isoefficiency**: $W = K \cdot T_o$ (How workload must grow to maintain efficiency)

> [!warning] Scaling Types
> - **Strong Scalability**: Fixed problem size, more processors. Limited by serial fraction.
> - **Weak Scalability**: Problem size grows with processors. Goal is constant efficiency.

## 3. Distributed Programming (MPI)
> [!code] Primitives
> - **Point-to-Point**: `MPI_Send`, `MPI_Recv`. Requires matching tag/rank.
> - **Collective**: `MPI_Bcast`, `MPI_Reduce`, `MPI_Scatter`, `MPI_Gather`, `MPI_Allreduce`.
> - **Advanced**: `MPI_Type_create_struct` ([[mpi-derived-datatypes]]), `MPI_Barrier`, `MPI_Exscan`.

> [!warning] MPI Common Pitfalls
> - **Deadlock**: Caused by mismatched sends/receives or incorrect barrier placement.
> - **I/O Restriction**: Typically only Process 0 has access to `stdin`.

## 4. Shared-Memory Programming (OpenMP)
> [!code] Constructs
> - **Parallel Region**: `#pragma omp parallel` ([[parallel-region]]).
> - **Worksharing**: `#pragma omp for` ([[parallel-loop-openmp]]), `sections`, `task`.
> - **Synchronization**: `critical`, `atomic`, `barrier`, `master`, `single`.
> - **Data-Sharing**: `shared`, `private`, `firstprivate`, `lastprivate`.

> [!warning] Optimization
> - Use `nowait` to remove implicit barriers.
> - Use `reduction(+:var)` instead of `critical` for high-performance summation.
> - **Scheduling**: `static` (predictable) vs `dynamic` (irregular).

## 5. Parallel Algorithms

### Sorting
- [[parallel-sorting-basics]]: Compare-and-Exchange, Merge-and-Split.
- [[odd-even-transposition-sort]]: Parallel version of Bubble Sort. $O(n)$ steps.
- [[bitonic-mergesort]]: $O(\log^2 n)$ parallel time using bitonic sequences.
- [[shearsort]]: 2D mesh sorting. Snakelike order.
- [[radix-sort]]: Non-comparison, stable sort. Uses `MPI_Exscan` and `MPI_Alltoallv`.

### Matrix Multiplication
- **Sequential Optimization**: [[spatial-locality]], Loop Reordering (ikj), [[matrix-multiplication-optimization]] (Tiling).
- **Algorithmic**: [[strassens-algorithm]] ($O(n^{2.81})$).
- **Distributed**:
    - **1D Striped**: $n/p$ ratio.
    - **Cannon's**: Skew-and-shift, $n/\sqrt{p}$ ratio.
    - **Fox's**: Broadcast-and-shift.
    - **SUMMA**: Outer-product based, pipelined panels.

## 6. Implementation & Environment
- [[slurm]]: Workload manager. `sbatch`, `srun`, `squeue`.
- [[pine-cluster]]: 16 nodes, 24 cores/node, InfiniBand.
- [[distributed-shared-memory]]: DSM, Write-Through vs. Write-Once protocols.
