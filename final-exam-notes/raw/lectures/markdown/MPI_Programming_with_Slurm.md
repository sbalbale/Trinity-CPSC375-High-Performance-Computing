# MPI Programming with Slurm

**Course:** CPSC 375: High-Performance Computing  
**Spring 2026** | Trinity College  
**Instructor:** Prof. Yoon

---

## Overview

This laboratory introduces **Message Passing Interface (MPI)** programming using the **Slurm workload manager**. Students will develop distributed-memory programs and use Slurm to orchestrate their execution across a high-performance computing cluster.

### The Pine Cluster

The departmental Pine cluster features:
- **16 Dell PowerEdge R730 nodes**
- **High-speed, low-latency network** infrastructure
- Each server includes:
  - **Dual Intel Xeon E5-2680 v3 CPUs** (24 physical cores each)
  - **60MB L3 cache** for optimized parallel data handling
  - **256GB DDR4 RDIMM RAM**
  - **2TB HDD** storage
  - **40Gbps Mellanox InfiniBand** backbone (low latency)
  - **Gigabit ethernet ports** for cluster management

---

## Learning Objectives

By the end of this session, you will:
- ✓ Understand how to write distributed-memory programs
- ✓ Use Slurm to orchestrate program execution across cluster hardware
- ✓ Manage resources and job scheduling
- ✓ Debug and monitor parallel applications

---

## Part 1: Environment Setup

### Step 1: Log In to the Head Node

Connect using your Trinity user ID:

```bash
$ ssh userid@pine.cs.trincoll.edu
```

When prompted for a password, enter: `cpsc375`

### Step 2: Change Your Password

For security, change your initial password:

```bash
[pine] $ passwd
```

### Step 3: Create a Workspace

Create a directory for your MPI work:

```bash
[pine] $ mkdir mpilab && cd mpilab
```

---

## Part 2: Writing the MPI Code

### Task: Hello from MPI

Write a basic C program that identifies which **rank** (process) is running on which **hostname** (physical node).

#### Create `mpi_hello.c`

```c
#include <mpi.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    MPI_Init(NULL, NULL);
    
    int size, rank;
    char hostname[256];
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    gethostname(hostname, 256);
    
    printf("Hello from Rank %d out of %d running on %s\n", rank, size, hostname);
    
    MPI_Finalize();
    return 0;
}
```

#### Compile

```bash
$ mpicc mpi_hello.c -o mpi_hello
```

**Note:** The `mpicc` compiler wrapper handles all necessary flags and libraries for MPI compilation.

---

## Part 3: Job Submission Using Slurm

Instead of running your code directly, you must request resources from the **Slurm scheduler**.

### Create a Batch Script: `submit.sh`

```bash
#!/bin/bash
#SBATCH --job-name=hello              # Name in the squeue
#SBATCH --nodes=3                     # Request 3 nodes
#SBATCH --ntasks-per-node=1           # 1 process per node
#SBATCH --output=result_%j.log        # Save output (%j = JobID)

# Run the MPI program you compiled
srun ./mpi_hello
```

### Submit to the Queue

```bash
$ sbatch submit.sh
```

### Check Job Status

```bash
$ squeue
```

### View Results

Once the job finishes, examine the output file:

```bash
$ cat result_[jobid].log
```

### Experimentation

Repeat the job submission with different configurations:
- Vary the number of nodes (up to 16)
- Vary the number of tasks per node (each Pine node has 24 physical cores)
- Observe how the rank and hostname change

---

## Part 4: Useful Slurm Commands

| Command | Purpose |
|---------|---------|
| `sbatch script.sh` | Submit a batch job |
| `squeue` | List all jobs in queue |
| `scancel [jobID]` | Cancel a job |
| `sacct` | Display accounting data for completed jobs |
| `sinfo` | Display cluster information |

### Example: Cancel a Running Job

If you spot a bug and need to kill a job:

```bash
$ squeue                    # Find your Job ID
$ scancel [JobID]          # Cancel it
```

---

## Part 5: Advanced Exercise - Over-requesting Resources

What happens when you request more resources than available?

### Task

1. Submit a job requesting more cores than a single node can provide:

   ```bash
   #SBATCH --ntasks-per-node=48   # More than 24 available cores
   ```

2. Monitor the status using `squeue`. What does it say?

3. Cancel the job using `scancel`.

### Expected Behavior

The job will remain in queue (PENDING state) until resources become available, or it will be rejected if the request exceeds total cluster capacity.

---

## Summary

You've learned:
- How to access the Pine cluster
- How to write a basic MPI program
- How to compile with `mpicc`
- How to submit jobs using Slurm batch scripts
- How to monitor and manage jobs

---

## Next Steps

Proceed to more advanced MPI programs:
- Point-to-point communication (MPI_Send, MPI_Recv)
- Collective operations (MPI_Bcast, MPI_Reduce, MPI_Gather)
- Asynchronous communication
- Performance optimization techniques
