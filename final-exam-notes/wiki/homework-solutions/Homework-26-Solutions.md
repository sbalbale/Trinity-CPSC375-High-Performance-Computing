---
aliases: [Homework 26 Solutions]
tags: [#homework/solutions, #course_hpc, #mpi]
sources: [Homework 26.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Homework 26 Solutions

> [!abstract]
> Solutions to the MPI process coordination and reduction exercises for Homework 26, including a token-passing mechanism for ordered output.

## Problem 1: Ordered Output (Token Passing)

**Requirement:** Force processes to print in order $0 \to N-1$ using `MPI_Send` and `MPI_Recv`.

> [!code] Implementation
> ```c
> #include <mpi.h>
> #include <stdio.h>
> 
> int main() {
>     int rank, size;
>     int token = 0;
>     MPI_Init(NULL, NULL);
>     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
>     MPI_Comm_size(MPI_COMM_WORLD, &size);
> 
>     if (rank != 0) {
>         // Wait for the token from the previous process
>         MPI_Recv(&token, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
>     }
> 
>     // Critical section: Print the message
>     printf("Greetings from process %d of %d\n", rank, size);
>     fflush(stdout); // Ensure output is flushed immediately
> 
>     if (rank < size - 1) {
>         // Send the token to the next process
>         MPI_Send(&token, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
>     }
> 
>     MPI_Finalize();
>     return 0;
> }
> ```

**Explanation:**
The processes form a logical chain. Each process (except Rank 0) blocks on `MPI_Recv` from its predecessor. Only after a process prints its message does it `MPI_Send` a dummy token to its successor, "unlocking" the next process. This guarantees sequential execution of the `printf` statement.

---

## Problem 2: Random Sum Reduction

**Requirement:** Each process generates a random integer. Compute sum at rank 0.

> [!code] Implementation
> ```c
> #include <mpi.h>
> #include <stdio.h>
> #include <stdlib.h>
> #include <time.h>
> 
> int main() {
>     int rank, size;
>     int local_val, total_sum;
> 
>     MPI_Init(NULL, NULL);
>     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
>     MPI_Comm_size(MPI_COMM_WORLD, &size);
> 
>     // Seed random number generator uniquely for each process
>     srand(time(NULL) + rank);
>     local_val = rand() % 100;
> 
>     printf("Process %d generated value: %d\n", rank, local_val);
> 
>     // Reduce all local_vals into total_sum on rank 0
>     MPI_Reduce(&local_val, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
> 
>     if (rank == 0) {
>         printf("-----\nTotal Sum on Rank 0: %d\n", total_sum);
>     }
> 
>     MPI_Finalize();
>     return 0;
> }
> ```

**Key Takeaways:**
1. **`srand(time(NULL) + rank)`**: Without the `+ rank`, all processes launched at the same second might generate the exact same "random" number.
2. **`MPI_Reduce`**: Efficiently combines the data. Note that `total_sum` is only valid on the root (Rank 0).
