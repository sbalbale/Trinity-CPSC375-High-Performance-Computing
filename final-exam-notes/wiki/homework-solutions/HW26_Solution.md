# Homework 26 — Solution
**CPSC 375: High-Performance Computing**

---

## Question 1

**Problem:** Modify the `mpi_hello.c` program so that processes print their messages in rank order (0 → N-1), instead of all at once. You may not use `sleep()`. Use MPI communication (such as `MPI_Send` and `MPI_Recv`) to enforce ordering. Run the program with multiple nodes and verify that the output always appears in strict rank order regardless of scheduling.

---

### Solution

The strategy is a **token-passing approach**: rank 0 prints first, then sends a "go-ahead" token to rank 1; rank 1 waits for the token, prints, then passes it to rank 2; and so on.

```c
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int token = 0;  // dummy message value

    if (rank == 0) {
        // Rank 0 prints first (no need to wait)
        printf("Hello from process %d of %d\n", rank, size);
        fflush(stdout);

        // Send token to rank 1 (if it exists)
        if (size > 1) {
            MPI_Send(&token, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        }
    } else {
        // Wait for the token from the previous rank
        MPI_Recv(&token, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Now safe to print
        printf("Hello from process %d of %d\n", rank, size);
        fflush(stdout);

        // Pass token to the next rank (if not the last)
        if (rank < size - 1) {
            MPI_Send(&token, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
```

### How It Enforces Order

- Rank 0 prints unconditionally, then sends a token to rank 1.
- Each subsequent rank **blocks on `MPI_Recv`** until it receives the token from its predecessor.
- Only after receiving the token does a process print and forward the token.
- This creates a **linear chain of dependencies**: rank k cannot print until rank k−1 has already printed and sent the token.

### Why `sleep()` Would Be Wrong

`sleep()` only introduces a fixed delay — it provides no guarantee of ordering. Under varying system load, a process that sleeps for 1 second might still print before another process that sleeps for 0.5 seconds if OS scheduling is unfavorable. `sleep()` is a heuristic, not a synchronization mechanism.

### Compilation and Running

```bash
mpicc -o mpi_hello_ordered mpi_hello_ordered.c
mpirun -np 8 ./mpi_hello_ordered

# Expected output (always in order):
# Hello from process 0 of 8
# Hello from process 1 of 8
# Hello from process 2 of 8
# ...
# Hello from process 7 of 8
```

---

## Question 2

**Problem:** Write a program where each process generates a random integer. Use `MPI_Reduce` to compute the total sum of all values at rank 0. Print both the local values (per process) and the final sum.

---

### Solution

```c
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Seed random number generator differently per process
    srand((unsigned int)(time(NULL) + rank * 1000));
    int local_val = rand() % 100;  // random integer in [0, 99]

    // Each process prints its local value
    // Use token passing to print in rank order (clean output)
    int token = 0;
    if (rank == 0) {
        printf("Process %d: local value = %d\n", rank, local_val);
        fflush(stdout);
        if (size > 1) MPI_Send(&token, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    } else {
        MPI_Recv(&token, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d: local value = %d\n", rank, local_val);
        fflush(stdout);
        if (rank < size - 1) MPI_Send(&token, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
    }

    // Wait for all prints to complete before reduce
    MPI_Barrier(MPI_COMM_WORLD);

    // Reduce: sum all local values at rank 0
    int global_sum = 0;
    MPI_Reduce(&local_val, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("\nTotal sum across all %d processes: %d\n", size, global_sum);
    }

    MPI_Finalize();
    return 0;
}
```

### Sample Output (4 processes)

```
Process 0: local value = 42
Process 1: local value = 17
Process 2: local value = 83
Process 3: local value = 29

Total sum across all 4 processes: 171
```

### Key Points

- **`srand(time(NULL) + rank * 1000)`**: Seeds each process with a unique value so they generate different random numbers. Without incorporating `rank`, all processes seeded with the same time would produce the same value.
- **`MPI_Reduce` with `MPI_SUM`**: Collects all `local_val` values and computes their sum at rank 0. Only rank 0's `global_sum` is meaningful after the call.
- **Token passing for printing**: Ensures local values are printed in rank order for readability before the reduction.
- **`MPI_Barrier` before `MPI_Reduce`**: Ensures all processes finish printing before the reduction begins, avoiding interleaved output with the final sum line.
