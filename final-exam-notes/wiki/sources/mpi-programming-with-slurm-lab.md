---
tags: [mpi, slurm, cluster, tutorial]
sources: [MPI Programming with Slurm.txt]
created: 2026-04-20
updated: 2026-04-20
---

# MPI Programming with Slurm

**Source:** MPI Programming with Slurm.txt
**Date ingested:** 2026-04-20
**Type:** Lab Manual / Tutorial

## Summary

This lab manual introduces the practicalities of running MPI programs on the **Pine Cluster** using the **Slurm** workload manager. It covers cluster hardware specifications, basic MPI code structure, compilation with `mpicc`, and the creation of Slurm batch scripts for job submission and monitoring.

## Key Claims

- The **Pine Cluster** provides 16 nodes, each with 24 physical cores and an InfiniBand backbone for high-performance communication.
- Parallel programs on a cluster must be managed by a scheduler (Slurm) to allocate resources fairly and efficiently.
- **srun** is the Slurm command used to launch MPI processes across the allocated nodes.
- Exceeding physical hardware limits (e.g., requesting 48 cores on a 24-core node) results in jobs being queued or failing.

## Entities Mentioned

- [[pine-cluster]] — The departmental computing cluster.
- [[slurm]] — The workload manager used for job scheduling.
- [[mpi]] — Distributed-memory programming API.

## Concepts Covered

- [[slurm-batch-scripts]] — Configuring resource requests.
- [[mpi-job-submission]] — `sbatch`, `squeue`, and `srun` workflow.
- [[distributed-memory-execution]] — Running code across multiple physical nodes.
- [[cluster-monitoring]] — Using `sacct` and `scancel`.
