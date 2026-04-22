---
aliases: [Homework 19 Solutions]
tags: [#homework/solutions, #course_hpc, #openmp]
sources: [HW19_Solution.md, Homework 19.pdf]
created: 2026-04-20
updated: 2026-04-21
---

# Homework 19 Solutions

> [!abstract]
> Solutions to advanced OpenMP exercises for Homework 19, focusing on atomic/critical performance, loop scheduling, and parallel sections.

## Problem 1: Atomic vs. Critical

**Goal:** Simulate 100,000 deposits to a single shared balance.

> [!code] Comparison
> ```c
> // Option A: Critical
> #pragma omp critical
> balance++;
> 
> // Option B: Atomic
> #pragma omp atomic
> balance++;
> ```

**Why `atomic` is faster:**
| Feature | `#pragma omp critical` | `#pragma omp atomic` |
|---|---|---|
| **Mechanism** | Software **mutex lock** (OS-level) | Single **hardware instruction** |
| **Overhead** | High (lock/unlock overhead) | Very low (CPU-level) |
| **Scope** | Entire code block | Single memory operation |

---

## Problem 2: Loop Scheduling

**Goal:** Loop from 0 to 20 with work proportional to $i$.

**Analysis:**
- **`schedule(static, 1)`**: Iterations are pre-assigned round-robin. In this case, the thread assigned $i=20$ will take significantly longer than others, causing **load imbalance**.
- **`schedule(dynamic, 1)`**: Iterations are assigned **on demand** at runtime. Threads handling small $i$ values finish quickly and pick up more work, ensuring all threads finish at roughly the same time.

---

## Problem 3: Parallel Sections

**Goal:** Calculate average and minimum of a dataset concurrently.

> [!code] Implementation
> ```c
> #pragma omp parallel sections
> {
>     #pragma omp section
>     { /* Calculate Average */ }
> 
>     #pragma omp section
>     { /* Find Minimum */ }
> }
> ```

**What do extra threads do?**
If 4 threads are available but only 2 sections are defined, the **2 surplus threads sit idle** at the implicit barrier until the active threads finish. `parallel sections` does not automatically subdivide tasks to use extra threads.
