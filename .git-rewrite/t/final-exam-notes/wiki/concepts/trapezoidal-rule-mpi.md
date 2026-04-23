---
aliases: [Numerical Integration in MPI, Parallel Trapezoid]
tags: [mpi, example, algorithm]
sources: [lec20.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Trapezoidal Rule Using MPI

> [!abstract]
> Parallelizing the **Trapezoidal Rule** for numerical integration is a classic example of **Domain Decomposition** in MPI. The interval $[a, b]$ is divided into smaller sub-intervals, and each process calculates the local area before sending it to a coordinator for aggregation.

## Core Mechanics

### 1. Decomposition
Divide $n$ trapezoids among $p$ processes:
- `local_n = n / p`
- `h = (b - a) / n`
- `local_a = a + my_rank * local_n * h`
- `local_b = local_a + local_n * h`

### 2. Local Computation
Each process runs a sequential Trapezoid function on its assigned sub-interval `[local_a, local_b]`.

### 3. Communication (Reduction)
- **Workers**: Use `MPI_Send` to transmit `local_integral` to Process 0.
- **Coordinator (P0)**: Uses a loop to `MPI_Recv` all local integrals and sums them.

> [!warning] Scaling Bottleneck
> Gathering results sequentially on Process 0 using `MPI_Recv` takes $O(p)$ time. This can be optimized to $O(\log p)$ using `MPI_Reduce` (a [[collective-communication]] operation).

## Connections
* **Prerequisites:** [[domain-decomposition]], [[point-to-point-communication]].
* **Used In:** Introductory parallel programming labs.
* **Contrasts With:** [[reduction-operation]] (which automates this pattern).
