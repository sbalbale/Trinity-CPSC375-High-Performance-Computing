# Homework 25 — Solution
**CPSC 375: High-Performance Computing**

---

## Question 1 — Odd-Even Transposition Sort

### Part A — Analysis

**Problem:** This algorithm alternates between odd and even phases. Explain why it is suitable for parallel execution, how many phases are needed to guarantee sorting, and what happens if one phase is skipped.

**Why it is suitable for parallel execution:**

Odd-even transposition sort decomposes into two alternating phases:
- **Even phase**: compare and swap pairs (0,1), (2,3), (4,5), …
- **Odd phase**: compare and swap pairs (1,2), (3,4), (5,6), …

Within each phase, **all comparisons are independent** — no pair shares an element with another pair in the same phase. This means every comparison-swap in a phase can be executed in parallel with O(1) time per phase (given enough processors). The algorithm requires only **nearest-neighbor communication**, making it ideal for distributed-memory architectures where each process holds one or more elements and only communicates with adjacent processes.

**Number of phases to guarantee sorting:**

For *n* elements, **n phases** (alternating odd and even) are required in the worst case. This is because the 0-1 principle for sorting networks guarantees correctness after n phases. Each phase can move an element at most one position toward its correct location, and in the worst case (e.g., reverse-sorted input), elements may need to travel up to n−1 positions.

**What happens if one phase is skipped:**

If a phase is skipped, some compare-and-swap operations are never performed, and the sequence may not be fully sorted. Specifically, elements that needed to cross the boundary of the skipped phase will remain out of order. The sort is **not guaranteed to terminate correctly** — the output may still have inversions.

---

### Part B — MPI Implementation

```c
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Compare-and-keep lower half (this process keeps the smaller values)
void compare_low(int *local, int *recv_buf, int local_n) {
    int *merged = malloc(2 * local_n * sizeof(int));
    // Merge local and received arrays
    int i = 0, j = 0, k = 0;
    while (i < local_n && j < local_n) {
        if (local[i] <= recv_buf[j]) merged[k++] = local[i++];
        else                          merged[k++] = recv_buf[j++];
    }
    while (i < local_n) merged[k++] = local[i++];
    while (j < local_n) merged[k++] = recv_buf[j++];
    // Keep the lower half
    memcpy(local, merged, local_n * sizeof(int));
    free(merged);
}

// Compare-and-keep upper half (this process keeps the larger values)
void compare_high(int *local, int *recv_buf, int local_n) {
    int *merged = malloc(2 * local_n * sizeof(int));
    int i = 0, j = 0, k = 0;
    while (i < local_n && j < local_n) {
        if (local[i] <= recv_buf[j]) merged[k++] = local[i++];
        else                          merged[k++] = recv_buf[j++];
    }
    while (i < local_n) merged[k++] = local[i++];
    while (j < local_n) merged[k++] = recv_buf[j++];
    // Keep the upper half
    memcpy(local, merged + local_n, local_n * sizeof(int));
    free(merged);
}

int cmp_int(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = 16;  // total elements (must be divisible by size for simplicity)
    int local_n = n / size;

    int *local = malloc(local_n * sizeof(int));
    int *recv_buf = malloc(local_n * sizeof(int));

    // Initialize with random data on rank 0, scatter
    if (rank == 0) {
        int *data = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) data[i] = rand() % 100;
        MPI_Scatter(data, local_n, MPI_INT, local, local_n, MPI_INT, 0, MPI_COMM_WORLD);
        free(data);
    } else {
        MPI_Scatter(NULL, local_n, MPI_INT, local, local_n, MPI_INT, 0, MPI_COMM_WORLD);
    }

    // Sort local data first
    qsort(local, local_n, sizeof(int), cmp_int);

    // n phases of odd-even transposition
    for (int phase = 0; phase < size; phase++) {
        int partner;
        if (phase % 2 == 0) {
            // Even phase: rank 0 pairs with 1, rank 2 with 3, etc.
            if (rank % 2 == 0) partner = rank + 1;
            else                partner = rank - 1;
        } else {
            // Odd phase: rank 1 pairs with 2, rank 3 with 4, etc.
            if (rank % 2 == 1) partner = rank + 1;
            else                partner = rank - 1;
        }

        if (partner < 0 || partner >= size) continue;  // no partner

        // Exchange local arrays with partner
        MPI_Sendrecv(local,    local_n, MPI_INT, partner, 0,
                     recv_buf, local_n, MPI_INT, partner, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Keep lower half if rank < partner, upper half otherwise
        if (rank < partner) compare_low(local,  recv_buf, local_n);
        else                compare_high(local, recv_buf, local_n);
    }

    // Gather sorted data at rank 0
    int *sorted = NULL;
    if (rank == 0) sorted = malloc(n * sizeof(int));
    MPI_Gather(local, local_n, MPI_INT, sorted, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Sorted: ");
        for (int i = 0; i < n; i++) printf("%d ", sorted[i]);
        printf("\n");
        free(sorted);
    }

    free(local); free(recv_buf);
    MPI_Finalize();
    return 0;
}
```

---

## Question 2 — Mergesort

### Part A — Analysis

**Problem:** Consider parallel mergesort where recursive calls execute in parallel. What is the parallel time complexity with unlimited processors, total work performed, and is it work-efficient?

**Parallel time complexity (unlimited processors):**

In parallel mergesort, at each level of recursion both halves are sorted simultaneously:
- Level 0: 1 merge of size n → O(n)
- Level 1: 2 merges of size n/2 → O(n/2) in parallel
- Level k: 2^k merges of size n/2^k → O(n/2^k) in parallel

The depth of recursion is log₂(n) levels. The merge at the top-level takes O(n) time sequentially, so the parallel time is dominated by the final merge:

**T_parallel = O(n)** with unlimited processors (the merge phase dominates after the recursive sorts run in parallel).

If we use a parallel merge as well (e.g., odd-even merge), each merge can run in O(log n), giving:

**T_parallel = O(log² n)**

**Total work performed:**

The total work is the same as sequential mergesort: at each of the log n levels, we perform O(n) total comparisons.

**W = O(n log n)**

**Is it work-efficient?**

An algorithm is work-efficient if its total work matches the optimal sequential algorithm. Since sequential mergesort is O(n log n) and our parallel version also performs O(n log n) total work, **yes, it is work-efficient**.

---

### Part B — MPI Mergesort Implementation

```c
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cmp_int(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

// Merge two sorted arrays into result
void merge(int *a, int na, int *b, int nb, int *result) {
    int i = 0, j = 0, k = 0;
    while (i < na && j < nb) {
        if (a[i] <= b[j]) result[k++] = a[i++];
        else               result[k++] = b[j++];
    }
    while (i < na) result[k++] = a[i++];
    while (j < nb) result[k++] = b[j++];
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = 16;
    int local_n = n / size;
    int *local = malloc(local_n * sizeof(int));

    // Scatter data from rank 0
    if (rank == 0) {
        int data[] = {15,3,8,1,12,7,4,10,6,2,14,9,5,11,0,13};
        MPI_Scatter(data, local_n, MPI_INT, local, local_n, MPI_INT, 0, MPI_COMM_WORLD);
    } else {
        MPI_Scatter(NULL, local_n, MPI_INT, local, local_n, MPI_INT, 0, MPI_COMM_WORLD);
    }

    // Each process sorts its local data
    qsort(local, local_n, sizeof(int), cmp_int);

    // Parallel merge phase: tree-based reduction
    // At each step, half the processes send their data to a partner and stop;
    // the other half receive and merge.
    int step = 1;
    int current_n = local_n;

    while (step < size) {
        if (rank % (2 * step) == 0) {
            // Receive from rank + step (if it exists)
            int partner = rank + step;
            if (partner < size) {
                int recv_n;
                MPI_Recv(&recv_n, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int *recv_buf = malloc(recv_n * sizeof(int));
                MPI_Recv(recv_buf, recv_n, MPI_INT, partner, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                int *merged = malloc((current_n + recv_n) * sizeof(int));
                merge(local, current_n, recv_buf, recv_n, merged);
                free(local); free(recv_buf);
                local = merged;
                current_n += recv_n;
            }
        } else if (rank % step == 0) {
            // Send to rank - step
            int partner = rank - step;
            MPI_Send(&current_n, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
            MPI_Send(local, current_n, MPI_INT, partner, 1, MPI_COMM_WORLD);
            break;  // this process is done
        }
        step *= 2;
    }

    if (rank == 0) {
        printf("Sorted array (%d elements): ", current_n);
        for (int i = 0; i < current_n; i++) printf("%d ", local[i]);
        printf("\n");
    }

    free(local);
    MPI_Finalize();
    return 0;
}
```

---

## Question 3

**Problem:** Explain why parallel sorting algorithms require synchronization between phases, how synchronization impacts performance, and which algorithm is most affected.

**Solution:**

**Why synchronization is required between phases:**

In parallel sorting, processes hold different portions of the data and must coordinate to determine when to perform comparisons, exchanges, or merges. Without synchronization:
- A process might compare its data with a neighbor that hasn't finished its previous phase yet, leading to incorrect comparisons.
- In odd-even sort, if a process starts the even phase before its neighbor finishes the odd phase, it may exchange based on stale data.

Synchronization ensures that **all processes complete one phase before any process starts the next**, preserving the correctness invariants the algorithm depends on.

**How synchronization impacts performance:**

Synchronization introduces **idle time** (barrier overhead). If processes complete their work at different times, the faster ones must wait at the barrier for the slowest. This waiting time grows with:
- **Load imbalance**: unequal data sizes or work-per-element
- **Number of processes**: larger communicators have higher barrier latency
- **Number of phases**: more phases → more synchronization points

The overhead is proportional to O(phases × barrier_latency). For algorithms with many phases, this can dominate the computation cost, especially on large clusters.

**Which algorithm is most affected:**

**Odd-even transposition sort** is most affected by synchronization overhead. It requires **n phases** (where n = number of processes or elements), and each phase requires a global synchronization point. With many processes, this means O(p) barrier operations, each costing O(log p) time, for a total synchronization overhead of O(p log p) — which can exceed the O(n log n) computation time for large p.

Parallel mergesort requires only **O(log p)** phases (one per tree level), so its synchronization overhead scales much more favorably.
