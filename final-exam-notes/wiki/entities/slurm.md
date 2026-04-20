---
aliases: [Slurm Workload Manager, Simple Linux Utility for Resource Management, Cluster Monitoring]
tags: [#exam/tool, #HPC]
sources: [MPI Programming with Slurm.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Slurm

> [!abstract]
> **Slurm** is an open-source, fault-tolerant, and highly scalable cluster management and job scheduling system for large and small Linux clusters. It is used to allocate resources and launch parallel jobs like MPI.

## Key Functions
1. **Resource Allocation**: Provides exclusive and/or non-exclusive access to resources (nodes, CPUs) for a duration of time.
2. **Framework**: Provides a framework for starting, executing, and monitoring work on the set of allocated nodes.
3. **Queueing**: Arbitrates contention for resources by managing a queue of pending work.

## Primary Commands
- `sbatch`: Submits a non-interactive batch script.
- `srun`: Launches a parallel job (typically inside an `sbatch` script).
- `squeue`: Reports the state of jobs.
- `scancel`: Cancels a pending or running job.
- `sacct`: Reports job accounting information.

## Connections
* **Manages:** [[pine-cluster]].
* **Used For:** [[mpi-job-submission]].
* **Component of:** Modern supercomputing clusters.
