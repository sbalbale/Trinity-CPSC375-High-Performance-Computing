---
aliases: [Homework 21 Solutions]
tags: [#homework/solutions, #course_hpc, #mpi]
sources: [HW21_Solution.md, Homework 21.pdf]
created: 2026-04-20
updated: 2026-04-21
---

# Homework 21 Solutions

> [!abstract]
> Solutions to MPI Collective Communication and Data Partitioning exercises for Homework 21.

## Question 1: Collective Misuse

**Bug:** `MPI_Reduce` called inside an `if (rank == 0)` block.
**Correction:** Collective operations must be called by **all processes** in the communicator.
> [!code] Corrected
> ```c
> MPI_Reduce(&x, &y, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
> ```

---

## Question 2: Matching by Order

**Claim:** MPI matches collectives by **call order**, not variable names.
If Process 0 calls `Reduce` on `a` then `c`, and Process 1 calls `Reduce` on `c` then `a`, the data will be mixed incorrectly. The runtime simply pairs the $N$-th collective call on every process together.

---

## Question 3: `Reduce` vs. `Allreduce`

| Feature | `MPI_Reduce` | `MPI_Allreduce` |
|---|---|---|
| **Result Location** | Root process only | **All** processes |
| **Use Case** | Final output to master | Convergence checks (all must see result) |

---

## Question 4: Partitioning Strategies

Array size 16, 4 processes (P0-P3).

- **Block**: P0: 0-3, P1: 4-7, P2: 8-11, P3: 12-15. (Best for **uniform work**).
- **Cyclic**: P0: 0,4,8,12, P1: 1,5,9,13... (Best for **irregular work** / load balancing).
- **Block-Cyclic (bs=2)**: P0: 0,1, 8,9, P1: 2,3, 10,11... (Middle ground).

---

## Question 5: Scatter and Gather

1. **`MPI_Scatter`**: Distributes `A = [0..7]` from P0 to 4 processes (2 elements each).
   - P0: [0,1], P1: [2,3], P2: [4,5], P3: [6,7]
2. **Multiply by 2**:
   - P0: [0,2], P1: [4,6], P2: [8,10], P3: [12,14]
3. **`MPI_Gather`**: Concatenates results back to P0.
   - **Result**: `[0, 2, 4, 6, 8, 10, 12, 14]`

---

## Question 6: Root Mismatch

**Bug**: `Reduce` root is rank 1, but rank 0 tries to print the result.
**Result**: Rank 0 prints garbage/uninitialized memory.
**Fix**: Ensure `Reduce` root rank matches the rank performing the I/O.

---

## Question 7: `Bcast` Invalidity

**Bug**: Different ranks calling `Bcast` with different `count` values.
> [!danger] Undefined Behavior
> Mismatched counts in collective calls lead to **data corruption or deadlocks**. All processes must call the collective with identical arguments (except for buffer pointers).
