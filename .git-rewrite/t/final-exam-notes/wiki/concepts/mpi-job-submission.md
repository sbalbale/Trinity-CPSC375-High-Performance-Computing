---
aliases: [sbatch workflow, running MPI on Pine, Distributed Memory Execution]
tags: [mpi, slurm, programming]
sources: [MPI Programming with Slurm.txt]
created: 2026-04-20
updated: 2026-04-20
---

# MPI Job Submission

> [!abstract]
> **MPI Job Submission** refers to the process of compiling and executing distributed programs on a cluster. This requires a transition from interactive development to a **batch scheduling** model managed by [[slurm]].

## Core Mechanics

### 1. Compilation
Distributed code must be compiled with an MPI-aware compiler to link the correct libraries.
> [!code]
> `mpicc source.c -o my_app`

### 2. The Batch Script
A shell script containing resource requests and the execution command.
> [!code] Example submit.sh
> ```bash
> #!/bin/bash
> #SBATCH --nodes=2
> #SBATCH --ntasks-per-node=4
> srun ./my_app
> ```

### 3. Submission and Monitoring
- **Submit**: `sbatch submit.sh` (returns a `JobID`).
- **Check Status**: `squeue` or `squeue -u <username>`.
- **Retrieve Results**: Standard output is usually directed to a file like `slurm-<JobID>.out`.

> [!warning] Hardware Constraints
> Requesting more tasks per node than physical cores available (e.g., 32 on a 24-core node) will typically cause the job to stay in the `PENDING` state indefinitely.

## Connections
* **Prerequisites:** [[mpi]], [[slurm]].
* **Used In:** All distributed programming assignments on the [[pine-cluster]].
* **Contrasts With:** Direct execution via `./my_app` (which only runs on the head node).
