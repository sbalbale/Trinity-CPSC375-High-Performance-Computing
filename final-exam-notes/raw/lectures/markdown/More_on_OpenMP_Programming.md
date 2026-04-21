# More on OpenMP Programming

**Course:** CPSC 375: High-Performance Computing  
**Spring 2026** | Trinity College  
**Instructor:** Prof. Yoon

---

## Overview

In this lab, you will explore **advanced OpenMP constructs** used to coordinate threads, control execution, and improve performance. You will learn how to:

- Synchronize threads using barriers
- Control execution flow with master and single directives
- Use atomic operations for thread-safe updates
- Manage loop scheduling
- Implement parallel sections and tasks
- Measure parallel performance

---

## Learning Objectives

- ✓ Master thread synchronization mechanisms
- ✓ Understand master vs. single directives
- ✓ Implement atomic operations correctly
- ✓ Control parallelism at fine and coarse granularity
- ✓ Measure and optimize performance

---

## Exercise 1: Using Barrier

A **barrier** forces all threads to wait until every thread reaches the same point in the code.

### Concept

Barriers are essential for **synchronizing parallel work**. Without barriers, threads might proceed at different rates, leading to race conditions or incorrect results.

### Code: `barrier.c`

```c
#include <stdio.h>
#include <omp.h>

int main() {
    #pragma omp parallel
    {
        printf("Thread %d before barrier\n", omp_get_thread_num());
        
        #pragma omp barrier
        
        printf("Thread %d after barrier\n", omp_get_thread_num());
    }
    
    return 0;
}
```

### Compile and Run

```bash
$ gcc -fopenmp -o barrier barrier.c
$ ./barrier
```

### Questions to Consider

1. **Do all threads print "before barrier" first?**
   - Yes, the barrier forces synchronization
   - No thread proceeds past the barrier until all reach it

2. **Why might barriers be necessary in parallel programs?**
   - To ensure all threads complete a stage before proceeding
   - To prevent race conditions in multi-stage algorithms
   - To coordinate computation and communication phases

---

## Exercise 2: Using Master

The **master directive** allows only the "master" thread (thread 0) to execute a block of code.

### Concept

Use `master` when only one thread should perform a task:
- I/O operations
- Accumulating final results
- Initializing shared data structures

### Code: `master.c`

```c
#include <stdio.h>
#include <omp.h>

int main() {
    #pragma omp parallel
    {
        printf("Thread %d working\n", omp_get_thread_num());
        
        #pragma omp master
        {
            printf("Master thread %d doing special work\n", omp_get_thread_num());
        }
    }
    
    return 0;
}
```

### Compile and Run

```bash
$ gcc -fopenmp -o master master.c
$ ./master
```

### Questions to Consider

1. **Which thread executes the master block?**
   - Only thread 0 (the master thread)

2. **How many times is the special message printed?**
   - Exactly once (only by thread 0)

3. **What happens to other threads?**
   - They skip the master block and continue
   - **Important:** No implicit barrier at the end of master!

---

## Exercise 3: Using Single

The **single directive** allows **any one thread** (not necessarily thread 0) to execute a block.

### Concept

- `single` is more flexible than `master`
- Useful when you don't care which thread does the work
- The thread that reaches it first executes the block
- Implicit barrier at the end (threads wait)

### Code: `single.c`

```c
#include <stdio.h>
#include <omp.h>

int main() {
    #pragma omp parallel
    {
        printf("Thread %d running\n", omp_get_thread_num());
        
        #pragma omp single
        {
            printf("One thread executes this section\n");
        }
    }
    
    return 0;
}
```

### Compile and Run

```bash
$ gcc -fopenmp -o single single.c
$ ./single
```

### Questions to Consider

1. **Which thread executes the single block?**
   - Whichever thread reaches it first
   - Different runs may show different threads

2. **Does it always use thread 0?**
   - No, that's the difference from `master`
   - Any thread can execute the single block

3. **Do other threads wait?**
   - Yes, there's an implicit barrier at the end

---

## Exercise 4: Using Atomic

The **atomic directive** protects a **single memory update operation** from race conditions.

### Concept

When multiple threads update the same variable, you need synchronization:
- **Incorrect:** Without protection, final value is unpredictable
- **Correct:** Use `atomic` to make the operation thread-safe

### Code: `atomic.c`

```c
#include <stdio.h>
#include <omp.h>

int main() {
    int counter = 0;
    
    #pragma omp parallel for
    for(int i = 0; i < 1000; i++)
    {
        #pragma omp atomic
        counter++;
    }
    
    printf("Counter = %d\n", counter);
    
    return 0;
}
```

### Compile and Run

```bash
$ gcc -fopenmp -o atomic atomic.c
$ ./atomic
```

### Expected Output

```
Counter = 1000
```

**Without the `atomic` directive**, the counter would be incorrect due to race conditions.

### Questions to Consider

1. **What is a race condition?**
   - Multiple threads access the same memory location
   - At least one performs a write
   - Without synchronization, the result is unpredictable

2. **Why is atomic important?**
   - Protects small, atomic operations (read-modify-write)
   - More efficient than locks for simple updates
   - Prevents data corruption

---

## Comparing Synchronization Mechanisms

| Directive | Purpose | Behavior | When to Use |
|-----------|---------|----------|-------------|
| `barrier` | Synchronize all threads | Waits for all threads to reach a point | Between computation phases |
| `master` | Only thread 0 executes | No implicit barrier at end | Initialization, final output |
| `single` | Any one thread executes | Implicit barrier at end | Load balancing, I/O |
| `atomic` | Protect single operation | Minimal overhead | Shared variable updates |
| `critical` | Protect code block | More overhead than atomic | Complex updates, multiple variables |

---

## Advanced Concepts

### Loop Scheduling

Control how iterations are distributed among threads:

```c
#pragma omp parallel for schedule(static)     // Divide evenly upfront
#pragma omp parallel for schedule(dynamic)    // Load-balanced, assign as available
#pragma omp parallel for schedule(guided)     // Hybrid approach
```

### Parallel Sections

Execute multiple blocks in parallel:

```c
#pragma omp parallel sections
{
    #pragma omp section
    {
        // Task A
    }
    
    #pragma omp section
    {
        // Task B
    }
}
```

### Tasks

Useful for recursive parallelism:

```c
#pragma omp parallel
#pragma omp single
{
    process_tree(root);  // Creates tasks recursively
}

void process_tree(node *n) {
    #pragma omp task
    {
        process_tree(n->left);
        process_tree(n->right);
    }
}
```

---

## Performance Measurement

Use OpenMP's built-in timing functions:

```c
#include <omp.h>

double start = omp_get_wtime();
// ... your parallel code ...
double end = omp_get_wtime();

printf("Elapsed time: %f seconds\n", end - start);
```

### Measuring Speedup

```c
double serial_time = ..., parallel_time = ...;
double speedup = serial_time / parallel_time;
double efficiency = speedup / num_threads;

printf("Speedup: %f\n", speedup);
printf("Efficiency: %f%%\n", efficiency * 100);
```

---

## Best Practices

1. **Use the simplest synchronization mechanism needed**
   - `atomic` is faster than `critical`
   - `single` is simpler than `master` if it works

2. **Minimize synchronization overhead**
   - Place synchronization outside inner loops
   - Batch operations when possible

3. **Understand implicit barriers**
   - Many directives (`for`, `single`, `sections`) have implicit barriers
   - Use `nowait` to skip the barrier if safe

4. **Test with different thread counts**
   - Set via environment variable: `export OMP_NUM_THREADS=4`
   - Profile before and after each change

---

## Exercises Summary

| Exercise | Directive | Key Concept |
|----------|-----------|------------|
| 1 | `barrier` | Synchronize all threads |
| 2 | `master` | Thread 0 only |
| 3 | `single` | Any thread |
| 4 | `atomic` | Race condition prevention |

---

## Additional Resources

- [OpenMP Official Site](https://www.openmp.org/)
- [OpenMP 5.0 Specification](https://www.openmp.org/spec-html/5.0/openmp.html)
- GCC OpenMP Documentation: `man libgomp`
- Run with debugging: `gcc -fopenmp -g -O0`

---

## Challenge Problems

1. Implement a parallel reduction using `atomic` instead of the `reduction` clause
2. Write a program that demonstrates load imbalance with different loop schedules
3. Create a parallel quicksort using `parallel sections`
4. Implement a barrier manually using OpenMP locks

---

## Summary

You've learned essential OpenMP synchronization and control mechanisms:
- **Barriers** for phase synchronization
- **Master/Single** for selective execution
- **Atomic** for race-free updates
- How to measure and optimize performance
