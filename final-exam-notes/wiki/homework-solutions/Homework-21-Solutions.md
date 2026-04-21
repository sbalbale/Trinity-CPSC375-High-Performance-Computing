---
aliases: [Homework 21 Solutions]
tags: [#homework/solutions, #course_hpc, #mpi]
sources: [Homework 21.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Homework 21 Solutions

> [!abstract]
> Solutions to the collective communication and data partitioning exercises for Homework 21, including code corrections and analysis of distribution patterns.

## Problem 1: Collective Participation

**What's wrong?**
The code attempts to use `MPI_Reduce` as a point-to-point operation. Rank 0 calls the reduction, while all other ranks call `MPI_Recv`. 

**Explanation:**
Collective operations like `MPI_Reduce` are **collective**. This means **every** process in the communicator must call the same routine. One process cannot "receive" the result of a reduction using a point-to-point receive.

> [!code] Corrected Version
> ```c
> // Everyone calls MPI_Reduce
> MPI_Reduce(&x, &y, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
> ```

---

## Problem 2: Call Order Dependency

**Scenario:**
- P0: a=1, c=2
- P1: a=2, c=1
- P2: a=1, c=2
- Call 1: `MPI_Reduce(&a, &b, ...)`
- Call 2: `MPI_Reduce(&c, &d, ...)`

**Results stored on process 0:**
- **b:** $1 + 2 + 1 = 4$
- **d:** $2 + 1 + 2 = 5$

**Why results depend on call order?**
MPI does not look at the names of the variables (`a` or `c`). It matches collective calls based on their **temporal order** within each process. If Process 1 were to call the reduction for `c` before the reduction for `a`, it would incorrectly contribute its `c` value to the global sum for `b`. This would lead to data corruption.

---

## Problem 3: Reduce vs. Allreduce

- **`MPI_Reduce`**: Combines values from all processes and returns the result **only to the root process**.
- **`MPI_Allreduce`**: Combines values from all processes and returns the result **to every process** in the communicator.

**Scenario for `MPI_Allreduce`:**
In an iterative solver (like Conjugate Gradient), you often need to calculate a global dot product or error norm. Since every process needs to know the error norm to decide whether the algorithm should terminate or continue to the next iteration, `MPI_Allreduce` is required.

---

## Problem 4: Data Partitioning Strategies

**Scenario:** Array of size 16, 4 processes (P0, P1, P2, P3).

| Strategy | P0 Indices | P1 Indices | P2 Indices | P3 Indices |
| :--- | :--- | :--- | :--- | :--- |
| **Block** | 0, 1, 2, 3 | 4, 5, 6, 7 | 8, 9, 10, 11 | 12, 13, 14, 15 |
| **Cyclic** | 0, 4, 8, 12 | 1, 5, 9, 13 | 2, 6, 10, 14 | 3, 7, 11, 15 |
| **Block-Cyclic (b=2)** | 0, 1, 8, 9 | 2, 3, 10, 11 | 4, 5, 12, 13 | 6, 7, 14, 15 |

**Justification:**
- **Uniform Work:** **Block partitioning** is more appropriate. It minimizes communication overhead and provides good spatial locality.
- **Varying Work:** **Cyclic partitioning** is more appropriate. If work increases with the index (e.g., more iterations for higher indices), a block distribution would leave P0 finishing early and P3 overwhelmed. Cyclic distribution intersperses "heavy" and "light" indices across all processors, ensuring better **load balancing**.

---

## Problem 5: Scatter and Gather

**Scenario:** $n=8, p=4, A = [0, 1, 2, 3, 4, 5, 6, 7]$.

1. **After `MPI_Scatter`**:
    - P0 receives: `[0, 1]`
    - P1 receives: `[2, 3]`
    - P2 receives: `[4, 5]`
    - P3 receives: `[6, 7]`
2. **After Multiplaying by 2**:
    - P0: `[0, 2]`
    - P1: `[4, 6]`
    - P2: `[8, 10]`
    - P3: `[12, 14]`
3. **After `MPI_Gather` (Result on P0)**:
    - `[0, 2, 4, 6, 8, 10, 12, 14]`

---

## Problem 6: Result Location Error

**What's wrong?**
The code specifies **Rank 1** as the root (`1`) of the reduction, but then tries to print the `result` on **Rank 0**.

**Explanation:**
In `MPI_Reduce`, only the root process receives the final computed value in its receive buffer. All other processes have undefined values in their `result` variable. Rank 0 is printing garbage data.

> [!code] Corrected Version
> ```c
> // Use Rank 0 as root
> MPI_Reduce(&local_val, &result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
> if (my_rank == 0) {
>     printf("Sum = %f\n", result);
> }
> ```

---

## Problem 7: Parameter Inconsistency

**Why is it invalid?**
The code provides **mismatched counts** to a collective operation. Rank 0 says it is broadcasting 1 integer, while all other ranks expect to receive 2 integers.

**Runtime Behavior:**
This results in **undefined behavior**. Possible outcomes include:
- **Deadlock**: Processes wait indefinitely for data that is never sent.
- **Runtime Error**: The MPI implementation detects the mismatch and aborts the program.
- **Corruption**: Rank 0 sends 4 bytes, but other ranks read 8 bytes from the network buffer, consuming data from the "next" message or reading junk memory.
