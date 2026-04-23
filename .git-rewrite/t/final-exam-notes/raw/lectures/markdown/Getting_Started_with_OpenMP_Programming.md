# Getting Started with OpenMP Programming

**Course:** CPSC 375: High-Performance Computing  
**Term:** Spring 2026

## Overview

This lab introduces OpenMP, a library for parallel programming in C. Through a series of small programs, you will create parallel regions, run loops across multiple threads, observe race conditions, and learn how synchronization and data-sharing clauses ensure correct results.

---

## Exercise 1: First OpenMP Program

### Objective
Learn to create a basic parallel region and identify thread IDs.

### Code
```c
#include <stdio.h>
#include <omp.h>

int main() {
    #pragma omp parallel
    {
        printf("Hello from thread %d\n", omp_get_thread_num());
    }
}
```

### Compilation and Execution
```bash
$ gcc -fopenmp -o hello_openmp hello_openmp.c
$ ./hello_openmp
```

### Key Concepts
- `#pragma omp parallel` creates a team of threads that execute the block
- `omp_get_thread_num()` returns the ID of the current thread
- All threads execute the same code block concurrently

### Questions to Consider
- How many lines are printed?
- Does the order change when you run again?
- What is the smallest thread ID?

---

## Exercise 2: Controlling Number of Threads

### Objective
Understand how to explicitly set the number of threads.

### Modified Code
```c
#include <stdio.h>
#include <omp.h>

int main() {
    omp_set_num_threads(4);
    #pragma omp parallel
    {
        printf("Thread %d running\n", omp_get_thread_num());
    }
}
```

### Compilation and Execution
```bash
$ gcc -fopenmp -o hello_openmp hello_openmp.c
$ ./hello_openmp
```

### Experiments
- Change the number from 4 to 2
- Change the number from 4 to 8
- Observe how the output changes

### Learning Points
- `omp_set_num_threads(n)` sets the number of threads in subsequent parallel regions
- The number of threads affects workload distribution
- Not all systems can efficiently support arbitrary thread counts

---

## Exercise 3: Parallel Loop

### Objective
Distribute loop iterations across multiple threads.

### Sequential Version
```c
#include <stdio.h>

int main() {
    for (int i=0; i<8; i++)
        printf("i = %d\n", i);
}
```

### Parallel Version
```c
#include <stdio.h>
#include <omp.h>

int main() {
    #pragma omp parallel for
    for (int i=0; i<8; i++)
    {
        printf("Thread %d handles i=%d\n", omp_get_thread_num(), i); 
    }
}
```

### Key Directive
- `#pragma omp parallel for` divides loop iterations among threads
- Each thread processes a subset of iterations independently
- The scheduling of iterations to threads is handled by OpenMP

### Questions to Explore
- Are loop iterations printed in order?
- Which thread handles each iteration?
- Does the output change each run?

---

## Exercise 4: Race Condition

### Objective
Observe and understand race conditions in concurrent code.

### Problematic Code (race.c)
```c
#include <stdio.h>
#include <omp.h>

int main() {
    int sum = 0;
    #pragma omp parallel for
    for (int i=0; i<1000; i++)
    {
        sum += 1;
    }
    printf("Sum = %d\n", sum);
}
```

### Compilation and Execution
```bash
$ gcc -fopenmp -o race race.c
$ ./race
```

### Expected vs. Actual Results
- **Expected result:** Sum = 1000
- **Actual result:** May be less (often much less)
- **Why:** Multiple threads modify `sum` simultaneously without synchronization

### Root Cause Analysis
The statement `sum += 1;` is not atomic and consists of three operations:
1. Read the current value of `sum`
2. Add 1 to it
3. Write the result back to `sum`

When multiple threads execute these steps concurrently without coordination, some increments are lost because threads overwrite each other's results.

### Key Insight
This is a **data race** - multiple threads access a shared variable without synchronization, leading to undefined behavior and incorrect results.

---

## Exercise 5: Fix with Critical Section

### Objective
Use critical sections to ensure mutual exclusion.

### Corrected Code (critical.c)
```c
#include <stdio.h>
#include <omp.h>

int main() {
    int sum = 0;
    #pragma omp parallel for
    for (int i=0; i<1000; i++)
    {
        #pragma omp critical
        {
            sum += 1;
        }
    }
    printf("Sum = %d\n", sum);
}
```

### Compilation and Execution
```bash
$ gcc -fopenmp -o critical critical.c
$ ./critical
```

### Result
```
Sum = 1000
```

### How It Works
- `#pragma omp critical` ensures only one thread executes the enclosed block at a time
- This guarantees that all increments to `sum` occur sequentially
- No increments are lost

### Trade-offs
- **Correctness:** Guaranteed
- **Performance:** Reduced because threads must wait their turn
- **Scalability:** Poor with many threads (serialization point)

---

## Exercise 6: Fix with Reduction

### Objective
Use reduction for efficient aggregate computation.

### Optimized Code (reduction.c)
```c
#include <stdio.h>
#include <omp.h>

int main() {
    int sum = 0;
    #pragma omp parallel for reduction(+:sum)
    for (int i=0; i<1000; i++)
    {
        sum += 1;
    }
    printf("Sum = %d\n", sum);
}
```

### Compilation and Execution
```bash
$ gcc -fopenmp -o reduction reduction.c
$ ./reduction
```

### Result
```
Sum = 1000
```

### How Reduction Works
A reduction clause combines private copies of a variable from multiple threads into a single final value using a specified operation.

**Process:**
1. Each thread gets its own private copy of `sum` (initialized to 0 for +)
2. Each thread increments its private copy without synchronization
3. At the end of the parallel region, all private copies are combined using the reduction operator (+)
4. The final result is stored in the original `sum` variable

### Supported Operations
- Addition: `+`
- Multiplication: `*`
- Maximum: `max`
- Minimum: `min`
- Bitwise AND: `&`
- Bitwise OR: `|`
- Bitwise XOR: `^`
- Logical AND: `&&`
- Logical OR: `||`

### Advantages Over Critical Section
- **Performance:** Much faster because threads don't wait for each other
- **Scalability:** Scales well with many threads
- **Correctness:** Still guaranteed

---

## Exercise 7: Shared Variable Behavior

### Objective
Understand how shared variables behave in parallel regions.

### Code (shared_example.c)
```c
#include <stdio.h>
#include <omp.h>

int main() {
    int x = 5;
    #pragma omp parallel
    {
        x = x + 1;
        printf("Thread %d: x = %d\n", omp_get_thread_num(), x);
    }
    printf("Final x = %d\n", x);
}
```

### Compilation and Execution
```bash
$ gcc -fopenmp -o shared_example shared_example.c
$ ./shared_example
```

### Default Data Sharing
- Variables declared before parallel regions are **shared** by default
- All threads see and modify the same memory location
- Modifications are visible to all threads

### Important Observations
- Threads may print different values due to non-deterministic execution order
- The final value of `x` is unpredictable because of race conditions
- Output will vary between runs

### Questions for Reflection
- Do all threads print the same value?
- What is the final value of x?
- Why might the output change each run?

---

## Exercise 8: Using Private Variables

### Objective
Give each thread its own copy of a variable.

### Code (private_example.c)
```c
#include <stdio.h>
#include <omp.h>

int main() {
    int x = 5;
    #pragma omp parallel private(x)
    {
        x = x + 1;
        printf("Thread %d: x = %d\n", omp_get_thread_num(), x);
    }
    printf("Final x = %d\n", x);
}
```

### Compilation and Execution
```bash
$ gcc -fopenmp -o private_example private_example.c
$ ./private_example
```

### How Private Works
- `private(x)` creates a separate copy of `x` for each thread
- Each thread's copy is **uninitialized** (contains garbage value)
- Modifications by one thread don't affect other threads
- The original `x` in main is not modified and retains its initial value

### Key Behavior
- Each thread reads an uninitialized value when it executes `x = x + 1;`
- The output values are unpredictable
- The final value printed in main remains **5**

### Questions to Answer
- What value does each thread print?
- Why might the values appear unpredictable?
- What is the final value of x in main?

---

## Exercise 9: Using firstprivate

### Objective
Combine privacy with initialization.

### Code (firstprivate_example.c)
```c
#include <stdio.h>
#include <omp.h>

int main() {
    int x = 5;
    #pragma omp parallel firstprivate(x)
    {
        x = x + 1;
        printf("Thread %d: x = %d\n", omp_get_thread_num(), x);
    }
    printf("Final x = %d\n", x);
}
```

### Compilation and Execution
```bash
$ gcc -fopenmp -o firstprivate_example firstprivate_example.c
$ ./firstprivate_example
```

### How firstprivate Works
- Each thread gets its own copy of `x`
- Each copy is **initialized with the value from the master thread** (5 in this case)
- Each thread adds 1, so each prints 6
- The original variable in main is not modified

### Key Differences
| Clause | Initialized? | Modification affects original? |
|--------|--------------|-------------------------------|
| `private(x)` | No (garbage) | No |
| `firstprivate(x)` | Yes (from original) | No |
| Shared | N/A | Yes |

### Questions to Explore
- What value does each thread print?
- Is x initialized for each thread?
- Does the value of x in main change?

---

## Exercise 10: Using lastprivate

### Objective
Preserve the last iteration's value.

### Code (lastprivate_example.c)
```c
#include <stdio.h>
#include <omp.h>

int main() {
    int x = 0;
    #pragma omp parallel for lastprivate(x)
    for (int i = 0; i < 8; i++)
    {
        x = i;
        printf("Thread %d: i=%d x=%d\n", omp_get_thread_num(), i, x); 
    }
    printf("Final x = %d\n", x);
}
```

### Compilation and Execution
```bash
$ gcc -fopenmp -o lastprivate_example lastprivate_example.c
$ ./lastprivate_example
```

### How lastprivate Works
- Each thread gets its own copy of `x`
- At the end of the loop, the value of `x` from the **logically last iteration** is copied back to the original variable
- The logically last iteration is the one with i=7

### What "Last Iteration" Means
- In a parallel loop `for (int i = 0; i < 8; i++)`, iteration i=7 is the last
- The value of `x` from whichever thread executes i=7 is saved
- This value overwrites the original `x` in main

### Critical Insight
- The iteration assignment depends on OpenMP's scheduling strategy
- The value propagated back is guaranteed to be from the last logical iteration, not from multiple threads

### Questions for Understanding
- Which value is printed as the final x?
- Why is it not the value from every thread?
- What does lastprivate guarantee?

---

## Exercise 11: Combining firstprivate and lastprivate

### Objective
Understand combined data-sharing clauses.

### Modified Loop
```c
#pragma omp parallel for firstprivate(x) lastprivate(x)
for (int i = 0; i < 8; i++)
{
    x = i;
    printf("Thread %d: i=%d x=%d\n", omp_get_thread_num(), i, x); 
}
```

### Combined Behavior
- `firstprivate(x)`: Each thread starts with `x` initialized to its original value
- `lastprivate(x)`: The value from the last iteration is copied back to the original variable

### Execution Flow
1. Each thread's `x` is initialized (usually to 0)
2. During the loop, `x` is modified as `x = i;`
3. After the loop, the final value of `x` from iteration i=7 is stored in the original variable

### Observations After Running
- What value does each thread start with?
- What is the final value after the loop?

---

## Exercise 12: Parallel Summation of Array

### Objective
Apply reduction to compute an array sum using OpenMP.

### Task Description
```c
// Create an array:
int A[1000];

// Initialize:
// A[i] = i;

// Write a program that computes the sum of an array using OpenMP and reduction.
```

### Solution Outline
```c
#include <stdio.h>
#include <omp.h>

int main() {
    int A[1000];
    
    // Initialize array
    for (int i = 0; i < 1000; i++)
        A[i] = i;
    
    // Parallel summation
    int sum = 0;
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < 1000; i++)
    {
        sum += A[i];
    }
    
    printf("Sum = %d\n", sum);
    
    return 0;
}
```

### Expected Result
- Sum of 0 + 1 + 2 + ... + 999 = 499,500

### Key Takeaways
- Reduction efficiently parallelizes aggregate computations
- No thread synchronization overhead like critical sections
- Each thread accumulates partial sums independently
- OpenMP combines partial sums at the end

---

## Summary of Key Concepts

### Parallel Regions
- `#pragma omp parallel` creates a team of threads
- All threads execute the same code block

### Loop Parallelization
- `#pragma omp parallel for` distributes iterations across threads
- Provides automatic workload distribution

### Synchronization
- `#pragma omp critical` ensures mutual exclusion
- Only one thread executes the critical section at a time

### Data Sharing Clauses
- **shared:** Variable is shared among all threads (default)
- **private:** Each thread has its own uninitialized copy
- **firstprivate:** Each thread has a copy initialized from the original
- **lastprivate:** The last iteration's value is copied back to the original
- **reduction:** Efficiently combines values from multiple threads

### Race Conditions
- Occur when multiple threads access shared data without synchronization
- Lead to unpredictable results
- Must be avoided through proper synchronization or data-sharing clauses

### Performance Considerations
- Critical sections serialize execution (poor scalability)
- Reduction is efficient for aggregate computations
- Private variables eliminate synchronization overhead
- Choose data-sharing clauses carefully for optimal performance

---

## References

- OpenMP Official Website: https://www.openmp.org/
- GCC OpenMP Documentation: https://gcc.gnu.org/projects/gomp/
- Course Website: https://www.cs.trincoll.edu/~pyoon/cpsc375/

