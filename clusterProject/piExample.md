### **Phase 1: Writing the Code**

This C code uses the Monte Carlo method to estimate Pi. It randomly generates points within a square and checks how many land inside an inscribed circle.

Save this as `pi_test.c`:

```c
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]) {
    long count = 0;
    double x, y;

    // 1. Initialize the MPI environment FIRST
    MPI_Init(&argc, &argv);

    int rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // 2. STRONG SCALING: Total points remains fixed at 2.4 Billion.
    long total_points = 2400000000L;
    long n_points = total_points / world_size; 

    // Start the timer
    double start_time = MPI_Wtime();

    // Give each core a unique seed so they don't generate identical points
    srand(time(NULL) + rank);

    // The math loop
    for (long i = 0; i < n_points; i++) {
        x = (double)rand() / RAND_MAX;
        y = (double)rand() / RAND_MAX;
        
        if (x * x + y * y <= 1.0) {
            count++;
        }
    }

    // 3. Sum all local 'count' variables into 'total_count' on Rank 0
    long total_count;
    MPI_Reduce(&count, &total_count, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime();

    // 4. Only the head node (Rank 0) prints the final result
    if (rank == 0) {
        double pi = 4.0 * total_count / (double)total_points;
        printf("\n============================================\n");
        printf("       THREE IDIOTS CLUSTER REPORT          \n");
        printf("============================================\n");
        printf("Estimated Pi: %f\n", pi);
        printf("Total Cores:  %d\n", world_size);
        printf("Total Points: %ld\n", total_points);
        printf("Points/Core:  %ld\n", n_points);
        printf("Time Taken:   %.4f seconds\n", end_time - start_time);
        printf("============================================\n\n");
    }

    MPI_Finalize();
    return 0;
}

```

---

### **Phase 2: Compiling with Intel OneAPI**

Because this code relies on `<mpi.h>`, you cannot use standard `gcc` or `icx`. You must use the Intel MPI compiler wrapper (`mpiicx`), which automatically links the complex networking and mathematical libraries.

**1. Load the Intel Environment:**

```zsh
source /opt/intel/oneapi/setvars.sh

```

**2. Compile with Optimization Flags:**

```zsh
mpiicx -O3 -xHost -qmkl pi_test.c -o pi_test

```

* `-O3`: Maximum speed optimization.
* `-xHost`: Optimizes the binary specifically for the architecture of the CPU compiling it.
* `-qmkl`: Links the Math Kernel Library.

**3. Distribute the Binary:**
If your working directory isn't a shared network drive (NFS), you must push the newly compiled file to the compute nodes:

```zsh
for node in node02 node03; do scp pi_test student@$node:~/; done

```

---

### **Phase 3: Interactive Execution**

You can run the code instantly from the terminal using `srun`. Because the code uses "Strong Scaling," testing on 1 core versus 24 cores will show a massive difference in execution time.

Make sure your terminal has the necessary environment variables set, then run your tests:

* **The 1-Core Baseline (~55 seconds):**
```zsh
srun -p compute -n 1 --mpi=pmi2 ./pi_test

```


* **The 24-Core Cluster (~2.5 seconds):**
```zsh
srun -p compute -n 24 --mpi=pmi2 ./pi_test

```



> **Note:** The `--mpi=pmi2` flag is mandatory for interactive `srun` sessions unless `MpiDefault=pmi2` is permanently set in your `slurm.conf`.

---

### **Phase 4: The Slurm Batch Script**

For actual production workloads, you should submit your jobs to the background using an `sbatch` script. This ensures the job runs even if you close your laptop, and it perfectly documents the exact environment used for the run.

Save this as `compute_pi.sh`:

```bash
#!/bin/bash
#SBATCH --job-name=idiot_pi
#SBATCH --partition=compute
#SBATCH --nodes=3
#SBATCH --ntasks-per-node=8
#SBATCH --output=pi_final.out
#SBATCH --error=pi_final.err

# 1. Load Intel Compiler Environment
source /opt/intel/oneapi/setvars.sh > /dev/null 2>&1

# 2. Network & MPI Handshake Fixes
export FI_PROVIDER=tcp
export FI_TCP_IFACE=10.0.0.0/24
export I_MPI_HYDRA_BOOTSTRAP=slurm
export I_MPI_PMI_LIBRARY=/usr/lib64/libpmi2.so.0

# 3. Execution
srun --mpi=pmi2 ./pi_test

```

### **Understanding the Script Breakdown**

**The Slurm Directives (`#SBATCH`)**

* `--nodes=3` & `--ntasks-per-node=8`: Tells the scheduler exactly how many physical machines to use and how many processes to pack onto each machine.
* `--output` & `--error`: Redirects the standard console text and error logs into easily readable `.out` and `.err` text files so you can check the results later.

**The Golden Environment Variables (`export`)**

* `FI_PROVIDER=tcp`: Forces Intel MPI to use standard Ethernet. This prevents the job from hanging if a node's firewall tries to block specialized Libfabric (InfiniBand) packets.
* `FI_TCP_IFACE=10.0.0.0/24`: Instead of naming a specific network interface (since your head node uses `enx...` and the compute nodes use `eno1`), this tells every node to automatically find and use whichever interface exists on the `10.0.0.x` subnet.
* `I_MPI_PMI_LIBRARY`: Tells Intel MPI exactly where to find the translation library it needs to talk to Slurm's process manager.

**To run the script:**

```zsh
sbatch compute_pi.sh

```