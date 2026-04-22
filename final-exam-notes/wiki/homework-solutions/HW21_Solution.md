# Homework 21 — Solution
**CPSC 375: High-Performance Computing**

---

## Question 1

**Problem:** What's wrong with the following program fragment? Correct the errors.

```c
if (my_rank == 0) {
    MPI_Reduce(&x, &y, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
} else {
    MPI_Recv(&y, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}
```

**Solution:**

There are two bugs:

1. **`MPI_Reduce` is a collective operation** — it must be called by *all* processes, not just rank 0. Placing it inside an `if (my_rank == 0)` block means only rank 0 calls it, causing a deadlock (the other processes call `MPI_Recv` and wait for a message that never arrives).

2. **The non-root processes should also call `MPI_Reduce`**, not `MPI_Recv`. `MPI_Reduce` handles the communication internally.

**Corrected code:**

```c
MPI_Reduce(&x, &y, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
```

All processes call `MPI_Reduce`. Only rank 0's `y` will contain the final sum; the `sendbuf` (`&x`) is used by all processes and the `recvbuf` (`&y`) is only meaningful at the root (rank 0).

---

## Question 2

**Problem:** Given two consecutive calls to `MPI_Reduce`:

```c
MPI_Reduce(&a, &b, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
MPI_Reduce(&c, &d, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
```

With values:
- Process 0: a=1, c=2
- Process 1: a=2, c=1
- Process 2: a=1, c=2

Determine the values stored in `b` and `d` on process 0 and explain why the result depends on call order rather than variable names.

**Solution:**

Each call to `MPI_Reduce` collects the *positional* send buffer across all processes in the order the calls are matched.

- **First `MPI_Reduce`**: collects `&a` from all processes → `b = 1 + 2 + 1 = 4`
- **Second `MPI_Reduce`**: collects `&c` from all processes → `d = 2 + 1 + 2 = 5`

**Why call order matters, not variable names:**  
MPI matches collective operations by the *order in which they are called* across processes, not by variable names or any symbolic label. The runtime has no knowledge of C variable names — it simply pairs the 1st collective call on each process together, the 2nd collective call together, etc. If processes called these in different orders (e.g., one process called with `&c` first), the values would be mixed incorrectly.

---

## Question 3

**Problem:** What is the difference between `MPI_Reduce` and `MPI_Allreduce`? Describe a scenario where `MPI_Allreduce` is necessary.

**Solution:**

| | `MPI_Reduce` | `MPI_Allreduce` |
|---|---|---|
| Result delivered to | Root process only | **All** processes |
| Subsequent broadcast needed? | Yes, if all need the result | No |
| Relative cost | Lower | Slightly higher (all-to-all communication) |

**`MPI_Allreduce` is necessary** when every process needs the reduced result to continue computation. For example, in an iterative solver (e.g., conjugate gradient), each process holds part of a residual vector and needs the **global dot product** to decide whether to stop iterating. Since every process must check the convergence criterion, all processes need the reduced value — `MPI_Reduce` followed by `MPI_Bcast` would achieve the same result but `MPI_Allreduce` does it in one optimized step.

---

## Question 4

**Problem:** For an array of size 16 distributed across 4 processes, describe the index assignments for block, cyclic, and block-cyclic (block size 2) partitioning. Which strategy is more appropriate when work per element is uniform vs. when it varies significantly?

**Solution:**

Array indices: 0–15, 4 processes (P0–P3).

**Block partitioning** (each process gets n/p = 4 contiguous elements):
| Process | Indices |
|---------|---------|
| P0 | 0, 1, 2, 3 |
| P1 | 4, 5, 6, 7 |
| P2 | 8, 9, 10, 11 |
| P3 | 12, 13, 14, 15 |

**Cyclic partitioning** (elements distributed round-robin):
| Process | Indices |
|---------|---------|
| P0 | 0, 4, 8, 12 |
| P1 | 1, 5, 9, 13 |
| P2 | 2, 6, 10, 14 |
| P3 | 3, 7, 11, 15 |

**Block-cyclic partitioning** (block size = 2, blocks distributed round-robin):
| Process | Indices |
|---------|---------|
| P0 | 0,1, 8,9 |
| P1 | 2,3, 10,11 |
| P2 | 4,5, 12,13 |
| P3 | 6,7, 14,15 |

**Which is better and when:**

- **Uniform work per element → Block partitioning** is preferred. It assigns contiguous memory to each process (better cache locality), minimizes communication (neighbors only need to exchange boundary elements), and is simple to implement. Load balance is perfect when work is uniform.

- **Variable work per element → Cyclic or block-cyclic partitioning** is preferred. If some elements require much more work than others (e.g., sparse matrix rows with varying non-zeros, triangular loops), block partitioning concentrates heavy work on one process. Cyclic distribution interleaves elements across processes, statistically distributing the heavy-work elements more evenly. Block-cyclic offers a tunable middle ground between locality (larger block size) and load balance (more blocks per process).

---

## Question 5

**Problem:** Assume n = 8, p = 4, and process 0 initially holds A = [0,1,2,3,4,5,6,7]. Determine what each process receives after applying `MPI_Scatter`. If each process multiplies its elements by 2 and calls `MPI_Gather`, what is the final array on process 0?

**Solution:**

**After `MPI_Scatter`** (root = process 0, each process receives n/p = 2 elements):

| Process | Elements received |
|---------|------------------|
| P0 | [0, 1] |
| P1 | [2, 3] |
| P2 | [4, 5] |
| P3 | [6, 7] |

`MPI_Scatter` distributes contiguous chunks of the send buffer from the root in rank order.

**Each process multiplies by 2:**

| Process | Local array after multiply |
|---------|--------------------------|
| P0 | [0, 2] |
| P1 | [4, 6] |
| P2 | [8, 10] |
| P3 | [12, 14] |

**After `MPI_Gather`** (root = process 0):

`MPI_Gather` collects the local arrays in rank order and concatenates them at the root.

**Final array on process 0: [0, 2, 4, 6, 8, 10, 12, 14]**

This is equivalent to doubling every element of the original array.

---

## Question 6

**Problem:** What is wrong with the following program? Correct it so it produces the intended result.

```c
double local_val, result;
MPI_Reduce(&local_val, &result, 1, MPI_DOUBLE, MPI_SUM, 1, MPI_COMM_WORLD);

if (my_rank == 0) {
    printf("Sum = %f\n", result);
}
```

**Solution:**

The bug is a **mismatch between the root rank in `MPI_Reduce` and the rank that prints the result**.

- `MPI_Reduce(..., 1, MPI_COMM_WORLD)` sends the result to **rank 1**.
- But the `printf` is guarded by `if (my_rank == 0)` — **rank 0** prints the result.
- Rank 0's `result` variable is never populated (its value is undefined/garbage after the reduce).

**Corrected code** — change the root to 0 to match the printing process:

```c
double local_val, result;
MPI_Reduce(&local_val, &result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

if (my_rank == 0) {
    printf("Sum = %f\n", result);
}
```

---

## Question 7

**Problem:** Explain why the following code is invalid and describe what could happen at runtime.

```c
if (my_rank == 0)
    MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);
else
    MPI_Bcast(&x, 2, MPI_INT, 0, MPI_COMM_WORLD);
```

**Solution:**

This code is **invalid** for two reasons:

1. **`MPI_Bcast` is a collective operation** that must be called identically by all processes. Here, rank 0 broadcasts **1 integer** while all other processes expect to receive **2 integers**. The `count` argument must match across all processes.

2. **`MPI_Bcast` cannot be conditionally wrapped** in an `if/else` where different ranks take different branches of a collective call. All processes must call the same collective with the same arguments.

**What could happen at runtime:**  
The behavior is undefined by the MPI standard. In practice, processes may experience a **type mismatch error**, **data corruption** (rank 0 sends 4 bytes but receivers read 8 bytes, picking up garbage data), or a **deadlock/hang** if the MPI implementation detects the size mismatch and stalls waiting for more data.

**Corrected code:**

```c
MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);
```

All processes call `MPI_Bcast` with the same arguments.
