---
aliases: [OpenMP vs MPI, Shared vs Distributed Synthesis]
tags: [#exam/theory, #synthesis, #hpc]
sources: [homework-16.md, homework-20.md, lec20.md, more-on-openmp-programming-lab.md]
created: 2026-04-21
updated: 2026-04-21
---

# Shared vs. Distributed Memory Models (OpenMP vs. MPI)

> [!abstract]
> This synthesis compares the two dominant paradigms in parallel computing: the **Shared Memory Model** (OpenMP) and the **Distributed Memory Model** (MPI). While OpenMP focuses on thread-level parallelism with implicit communication, MPI enables process-level parallelism with explicit message passing across networked nodes.

## Fundamental Comparison

| Feature | OpenMP (Shared) | MPI (Distributed) |
|---|---|---|
| **Memory Access** | Global Address Space (Unified) | Private Address Spaces (Partitioned) |
| **Communication** | **Implicit** (via shared variables) | **Explicit** (via message passing) |
| **Entity** | Threads within a single process | Processes across multiple nodes |
| **Scaling** | Limited by single-node bus/memory | **Highly Scalable** to thousands of nodes |
| **Overhead** | Synchronization (Locks, Atomics) | Data Movement (Network Latency) |

## Communication Patterns

### OpenMP: The Race Condition Risk
In [[shared-memory-model]], communication is "free" in terms of syntax (just read/write a variable), but expensive in terms of correctness.
- **Problem**: Multiple threads writing to `sum += 1` without protection leads to a [[race-condition]].
- **Solution**: Use [[omp-synchronization]] (Atomics, Critical Sections) or [[reduction-operation]].

### MPI: The Deadlock Risk
In [[distributed-memory-model]], communication must be manually orchestrated.
- **Matching**: Every `MPI_Send` must have a matching `MPI_Recv` with correct tags and ranks.
- **Problem**: Symmetric blocking sends (P0 sends to P1 while P1 sends to P0) cause **deadlock** as both wait for a receiver that is currently sending.
- **Solution**: Use non-blocking calls (`MPI_Isend`) or alternate send/recv orders.

## Strategic Decision Matrix

> [!warning] When to Use Which?
> - **Use OpenMP** when: Your problem fits on one powerful server (e.g., Lenovo P340), requires heavy data sharing, and you want incremental parallelization via pragmas.
> - **Use MPI** when: Your problem size exceeds one node's RAM, requires massive scalability, or you are running on a cluster like [[pine-cluster]].

## Hybrid Programming
Modern HPC often uses **MPI + OpenMP**:
- **MPI** handles inter-node communication (between servers).
- **OpenMP** handles intra-node parallelism (utilizing multiple cores/threads within each server).
- This approach maximizes hardware utilization of modern multi-core clusters.

## Connections
* **Concepts**: [[shared-memory-model]], [[distributed-memory-model]], [[spmd]], [[race-condition]].
* **Entities**: [[openmp]], [[mpi]].
