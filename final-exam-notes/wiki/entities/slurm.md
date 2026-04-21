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
- `sacct`: Reports job accounting information for completed jobs.
- `sinfo`: Displays general information about cluster nodes and partitions.

> [!warning] Over-requesting Resources
> If a job requests more tasks per node than physical cores available (e.g., 48 on a 24-core node), the job will remain in the **PENDING** state indefinitely until resources become available or will be rejected if the request exceeds total cluster capacity.

## Connections
* **Manages:** [[pine-cluster]].
* **Used For:** [[mpi-job-submission]].
* **Component of:** Modern supercomputing clusters.
