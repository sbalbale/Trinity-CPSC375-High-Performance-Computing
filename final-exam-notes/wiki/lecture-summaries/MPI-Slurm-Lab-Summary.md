---
aliases: [MPI Slurm Lab Summary, Cluster Computing Lab Summary]
tags: [#lecture/detailed, #CPSC375, #mpi, #slurm]
sources: [MPI Programming with Slurm.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Lab: MPI Programming with Slurm

> [!abstract]
> This lab covers the setup and execution of distributed-memory programs on the **Pine Cluster** using **Slurm**. It focuses on the transition from writing code to orchestrating its execution across multiple physical nodes.

## The Pine Cluster
The cluster consists of 16 Dell PowerEdge R730 nodes:
- **CPU**: Dual Intel Xeon E5-2680 v3 (24 cores per node).
- **RAM**: 256GB DDR4.
- **Network**: 40Gbps InfiniBand (high bandwidth, low latency).

## MPI Execution Workflow
1. **Writing**: Standard MPI C code using `mpi.h`.
2. **Compiling**: Use `mpicc` to link the MPI libraries.
3. **Scheduling**: Create a Slurm batch script to request nodes and tasks.
4. **Launching**: Use `srun` inside the script to start the parallel processes.

## Slurm Commands
| Command | Purpose |
| :--- | :--- |
| `sbatch script.sh` | Submit a job script to the queue. |
| `squeue` | View current status of all jobs in the queue. |
| `sacct` | View accounting and historical data for finished jobs. |
| `scancel [JobID]` | Terminate a running or queued job. |

## Anatomy of an #SBATCH Script
> [!code] Example Script
> ```bash
> #!/bin/bash
> #SBATCH --nodes=4            # Request 4 nodes
> #SBATCH --ntasks-per-node=12 # 12 processes per node (48 total)
> #SBATCH --output=log_%j.txt  # Output redirected to log_[JobID].txt
> 
> srun ./my_mpi_program
> ```
The `#SBATCH` directives are comments interpreted by the Slurm manager, while `srun` is the command that actually triggers the MPI environment across the allocated resources.
