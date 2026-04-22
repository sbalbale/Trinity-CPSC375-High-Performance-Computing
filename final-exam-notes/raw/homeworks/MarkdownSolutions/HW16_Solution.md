# Homework 16 — Solution
**CPSC 375: High-Performance Computing | Spring 2026**

---

## Problem 1

**Consider the following C code segment:**

```c
int a = 10;
int b = 20;
#pragma omp parallel private(a)
{
    int c = 30;
    a = omp_get_thread_num();
    b = b + a;
}
```

### Part A — Shared Variables

**Which variables are shared across all threads?**

`b` is shared. It is declared outside the parallel region and is **not** listed in the `private()` clause, so every thread accesses the same memory address for `b`.

### Part B — Private Variables

**Which variables are private to each thread?**

- `a` — explicitly declared `private(a)` in the `#pragma omp parallel` directive. Each thread gets its own uninitialized copy of `a`.  
- `c` — declared **inside** the parallel block. Variables declared within a parallel region are automatically private to each thread (they live on each thread's own stack).

### Part C — Race Condition with 4 Threads

**If this code runs with 4 threads, what is the value of `b` at the end? Explain why this results in a race condition.**

The final value of `b` is **indeterminate** — it cannot be reliably predicted.

**Why this is a race condition:**  
Each thread executes the compound statement `b = b + a` where `b` is a shared variable. This statement compiles to three separate operations:

1. **Read** the current value of `b` from shared memory  
2. **Compute** `b + a` (using the thread's private `a`, which equals its thread ID: 0, 1, 2, or 3)  
3. **Write** the result back to `b` in shared memory

Without any synchronization, threads can interleave these steps arbitrarily. For example, Thread 0 and Thread 1 might both read `b = 20` simultaneously, then compute `20+0=20` and `20+1=21` respectively, and both write back — whichever writes last "wins," erasing the other's update.

Possible values of `b` range from 20 (Thread 0 wins every race) to 20+0+1+2+3 = 26 (if all updates succeed without conflict), but there is no guarantee which outcome occurs. The undefined result is a **race condition** caused by concurrent unsynchronized writes to the shared variable `b`.

---

## Problem 2

**Write a short C code snippet using OpenMP that:**
- **A.** Sets the number of threads to 8 using a library function.
- **B.** Uses a parallel directive to have each thread print: "Hello from thread [ID] out of [Total] threads."

### Solution

```c
#include <stdio.h>
#include <omp.h>

int main() {
    // Part A: Set the number of threads to 8
    omp_set_num_threads(8);

    // Part B: Launch parallel region; each thread prints its greeting
    #pragma omp parallel
    {
        int id    = omp_get_thread_num();   // This thread's ID (0-based)
        int total = omp_get_num_threads();  // Total threads in this team
        int procs = omp_get_num_procs();    // Physical processors available

        printf("Hello from thread %d out of %d threads. "
               "(%d processors available)\n", id, total, procs);
    }

    return 0;
}
```

**Compile and run:**
```bash
gcc -fopenmp -o hw16 hw16.c
./hw16
```

**Expected output (order may vary):**
```
Hello from thread 0 out of 8 threads. (16 processors available)
Hello from thread 3 out of 8 threads. (16 processors available)
Hello from thread 1 out of 8 threads. (16 processors available)
...
```

**Function explanations:**
- `omp_set_num_threads(8)` — sets the number of threads to use in subsequent parallel regions (called before the parallel block).
- `omp_get_thread_num()` — returns the calling thread's ID within the current team (0 through N-1).
- `omp_get_num_threads()` — returns the total number of threads currently active in the team.
- `omp_get_num_procs()` — returns the number of physical processors (hardware threads) available to the program.
