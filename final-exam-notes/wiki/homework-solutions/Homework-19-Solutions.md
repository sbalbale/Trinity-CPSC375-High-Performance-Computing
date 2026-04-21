---
aliases: [Homework 19 Solutions]
tags: [#homework/solutions, #course_hpc, #openmp]
sources: [Homework 19.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Homework 19 Solutions

> [!abstract]
> Solutions to the advanced OpenMP coordination and performance exercises for Homework 19, featuring performance benchmarks for synchronization and scheduling strategies.

## Problem 1: Atomic vs. Critical

**Scenario:** 100,000 deposits of $1 to a single integer `balance`.

> [!code] Implementation
> ```c
> int balance = 0;
> double start = omp_get_wtime();
> #pragma omp parallel for
> for (int i = 0; i < 100000; i++) {
>     #pragma omp atomic // or critical
>     balance++;
> }
> double end = omp_get_wtime();
> ```

**Which one is faster?**
The version using **`#pragma omp atomic`** is significantly faster.

**Why?**
- **`atomic`**: Leverages specific hardware instructions (e.g., `LOCK INC` on x86) to perform the update in a single uninterruptible step. It has very low overhead.
- **`critical`**: Implements a general-purpose **mutual exclusion lock** (mutex). It involves more complex logic, potentially including operating system calls to manage thread queues, which is much slower for a simple scalar increment.

---

## Problem 2: Loop Scheduling

**Scenario:** Loop from $i=0$ to $20$ with work proportional to $i$.

### A. Static Scheduling (`schedule(static, 1)`)
In the static version, iterations are assigned in a round-robin fashion before the loop starts.
- Thread 0: $0, 4, 8, 12, 16, 20$
- Thread 1: $1, 5, 9, 13, 17$
- ...
**Result:** Threads assigned higher indices (like Thread 0 getting 20) will finish **much later** than others because their workload is larger. The total execution time is determined by the most heavily burdened thread.

### B. Dynamic Scheduling (`schedule(dynamic, 1)`)
In the dynamic version, iterations are assigned at runtime. When a thread finishes an iteration, it "asks" for the next one.

**How dynamic fixes this:**
Dynamic scheduling ensures **load balancing**. If Thread 1 finishes its "light" work (e.g., $i=1, 2$) quickly, it will move on to take more iterations while other threads are still busy with "heavy" work. This keeps all processors utilized and reduces the total wall-clock time.

---

## Problem 3: Parallel Sections

**Requirement:** Compute average and minimum concurrently.

> [!code] Implementation
> ```c
> #pragma omp parallel sections
> {
>     #pragma omp section
>     {
>         calculate_average(data);
>     }
>     #pragma omp section
>     {
>         find_minimum(data);
>     }
> }
> ```

**Question:** If you have 4 threads available but only 2 sections defined, what do the other 2 threads do?

**Answer:**
The other 2 threads will be **idle** for the duration of the `sections` construct. Since there are only 2 distinct units of work defined, OpenMP can only assign work to 2 threads. The remaining threads reach the construct, find no sections available to execute, and skip to the implicit barrier at the end of the `sections` block.

> [!warning] Efficiency
> Using `sections` with a high thread count but few sections results in poor **parallel efficiency**. For better utilization, one should use data parallelism ([[parallel-loop-openmp]]) if the operations allow it.
