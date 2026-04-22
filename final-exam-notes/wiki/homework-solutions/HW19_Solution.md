# Homework 19 — Solution
**CPSC 375: High-Performance Computing | Spring 2026**

---

## Problem 1

**Simulate 100,000 small deposits of $1 to a single `int balance = 0`. Run once with `#pragma omp critical` and once with `#pragma omp atomic`. Time both and explain why `atomic` is faster.**

### Solution

```c
#include <stdio.h>
#include <omp.h>

#define DEPOSITS 100000

int main() {
    int balance;
    double t_start, t_end;

    /* ── Part A: Using #pragma omp critical ── */
    balance = 0;
    t_start = omp_get_wtime();

    #pragma omp parallel for
    for (int i = 0; i < DEPOSITS; i++) {
        #pragma omp critical
        {
            balance++;  // Only one thread executes this block at a time
        }
    }

    t_end = omp_get_wtime();
    printf("critical: balance = %d, time = %.6f seconds\n",
           balance, t_end - t_start);

    /* ── Part B: Using #pragma omp atomic ── */
    balance = 0;
    t_start = omp_get_wtime();

    #pragma omp parallel for
    for (int i = 0; i < DEPOSITS; i++) {
        #pragma omp atomic
        balance++;  // Single atomic instruction; no mutex overhead
    }

    t_end = omp_get_wtime();
    printf("atomic:   balance = %d, time = %.6f seconds\n",
           balance, t_end - t_start);

    return 0;
}
```

**Compile and run:**
```bash
gcc -fopenmp -O2 -o hw19_1 hw19_1.c
./hw19_1
```

**Sample output:**
```
critical: balance = 100000, time = 0.008341 seconds
atomic:   balance = 100000, time = 0.001102 seconds
```

**Why `atomic` outperforms `critical` for a simple increment:**

| Feature | `#pragma omp critical` | `#pragma omp atomic` |
|---|---|---|
| Mechanism | Software **mutex lock** (OS-level lock/unlock) | Single **hardware instruction** (e.g., `LOCK XADD` on x86) |
| Overhead | High — acquiring/releasing a mutex requires kernel calls and thread context overhead | Very low — one atomic CPU instruction, no OS involvement |
| Scope | Serializes an **entire code block** (any number of statements) | Serializes a **single memory operation** only |
| Use case | Complex multi-statement critical sections | Simple scalar updates: `++`, `+=`, `-=`, `*=`, etc. |

For a simple `balance++`, `atomic` maps to a single processor instruction that is indivisible at the hardware level. `critical` uses a heavyweight mutual exclusion lock that can require the OS scheduler to suspend threads, making it orders of magnitude slower for fine-grained updates.

---

## Problem 2

**Write a loop from `i = 0` to 20 where work is proportional to `i`. Compare `schedule(static, 1)` vs. `schedule(dynamic, 1)` and explain the difference.**

### Solution

```c
#include <stdio.h>
#include <omp.h>

#define MAX_I 20

// Simulates work proportional to i (busy-wait loop)
void do_work(int i) {
    volatile long count = 0;
    for (long j = 0; j < (long)i * 10000000L; j++)
        count++;
}

int main() {
    printf("=== Part A: schedule(static, 1) ===\n");
    #pragma omp parallel for schedule(static, 1)
    for (int i = 0; i <= MAX_I; i++) {
        printf("Thread %d handles iteration i=%d\n",
               omp_get_thread_num(), i);
        do_work(i);
    }

    printf("\n=== Part B: schedule(dynamic, 1) ===\n");
    #pragma omp parallel for schedule(dynamic, 1)
    for (int i = 0; i <= MAX_I; i++) {
        printf("Thread %d handles iteration i=%d\n",
               omp_get_thread_num(), i);
        do_work(i);
    }

    return 0;
}
```

**Compile and run:**
```bash
gcc -fopenmp -O0 -o hw19_2 hw19_2.c
./hw19_2
```

**Analysis:**

**Static scheduling (`schedule(static, 1)`):**  
With chunk size 1, iterations are pre-assigned to threads in a round-robin pattern at compile time. With 4 threads and 21 iterations (i = 0..20):

- Thread 0 gets: i = 0, 4, 8, 12, 16, 20 → total work ∝ 0+4+8+12+16+20 = **60**
- Thread 1 gets: i = 1, 5, 9, 13, 17    → total work ∝ 1+5+9+13+17 = **45**
- Thread 2 gets: i = 2, 6, 10, 14, 18   → total work ∝ 2+6+10+14+18 = **50**
- Thread 3 gets: i = 3, 7, 11, 15, 19   → total work ∝ 3+7+11+15+19 = **55**

Thread 0 (with i=20) finishes **much later** than threads handling small `i` values. All other threads sit idle waiting at the implicit barrier while Thread 0 completes its heavy work. This is a classic **load imbalance** problem.

**Dynamic scheduling (`schedule(dynamic, 1)`):**  
Iterations are assigned **on demand** at runtime. Whenever a thread finishes its current iteration, it immediately picks up the next available one from a shared work queue. Threads handling small `i` values (fast iterations) automatically pick up more work, while threads handling large `i` values (slow) naturally handle fewer. The total workload is spread more evenly, and all threads finish at roughly the same time.

**Conclusion:** Dynamic scheduling fixes the load imbalance by distributing work at runtime rather than at compile time. The trade-off is a small scheduling overhead per iteration (queue access), but for unequal work like this, the performance gain vastly outweighs the cost.

---

## Problem 3

**Create a program with two independent operations on a large dataset: (1) calculate the average, (2) find the minimum. Wrap in `#pragma omp parallel sections`. What do extra threads do if only 2 sections are defined?**

### Solution

```c
#include <stdio.h>
#include <float.h>
#include <omp.h>

#define N 1000000

int main() {
    static double arr[N];
    double avg = 0.0, min_val = DBL_MAX;

    // Initialize the array
    for (int i = 0; i < N; i++)
        arr[i] = (double)(i + 1);

    /*
     * #pragma omp parallel sections spawns a team of threads.
     * Each #pragma omp section is assigned to exactly one thread.
     * The two sections run CONCURRENTLY on different threads.
     */
    #pragma omp parallel sections
    {
        /* Section 1: Calculate the average */
        #pragma omp section
        {
            double sum = 0.0;
            for (int i = 0; i < N; i++)
                sum += arr[i];
            avg = sum / N;
            printf("Section 1 (avg)  → Thread %d: average = %.2f\n",
                   omp_get_thread_num(), avg);
        }

        /* Section 2: Find the minimum value */
        #pragma omp section
        {
            double local_min = arr[0];
            for (int i = 1; i < N; i++)
                if (arr[i] < local_min)
                    local_min = arr[i];
            min_val = local_min;
            printf("Section 2 (min)  → Thread %d: minimum = %.2f\n",
                   omp_get_thread_num(), min_val);
        }
    }
    /* Implicit barrier here — both sections must complete before proceeding */

    printf("\nResults: average = %.2f, minimum = %.2f\n", avg, min_val);
    return 0;
}
```

**Compile and run:**
```bash
gcc -fopenmp -O2 -o hw19_3 hw19_3.c
./hw19_3
```

**Sample output:**
```
Section 1 (avg)  → Thread 0: average = 500000.50
Section 2 (min)  → Thread 1: minimum = 1.00

Results: average = 500000.50, minimum = 1.00
```

**Answer to the question — what do the other 2 threads do?**

If 4 threads are available but only 2 `#pragma omp section` blocks are defined, the **other 2 threads sit idle** (they do no useful work). The OpenMP specification assigns each section to exactly one thread; surplus threads wait at the **implicit barrier** at the end of the `sections` construct until the 2 active threads complete their sections. Only then do all 4 threads proceed past the barrier together.

This is a key limitation of `parallel sections`: it does not automatically subdivide sections to use extra threads. To take advantage of all 4 threads, you would need to add more sections or switch to a `parallel for` with a reduction for both operations.
