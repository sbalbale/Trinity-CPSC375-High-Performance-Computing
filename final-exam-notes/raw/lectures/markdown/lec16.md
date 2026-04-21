# Lecture 16: Shared Memory Programming Using OpenMP

**Course:** CPSC 375 High-Performance Computing  
**Topic:** Shared-Memory Model and OpenMP Basics

---

## Shared-Memory Model

### Architecture

```
┌─────────────┐
│  Processor  │
└──────┬──────┘
       │
┌──────┴──────────────────────┐
│       Shared Memory         │
├─────────────────────────────┤
│     Shared Variables        │
└─────────────────────────────┘
```

### Key Characteristics

1. **Single Memory Address Space**
   - All processors access the same memory
   - Variables are shared among all processors
   - No explicit data transfer needed

2. **Synchronization**
   - Processors interact and synchronize through **shared variables**
   - Changes by one processor are visible to others
   - Requires careful coordination to avoid conflicts

3. **Advantages**
   - Easy programming model (familiar to sequential programmers)
   - Implicit data sharing (no explicit sends/receives)
   - Natural for iterative methods and shared-data algorithms

4. **Challenges**
   - Race conditions when multiple processors access same variable
   - Cache coherency issues in multiprocessor systems
   - Synchronization overhead

---

## OpenMP

### What is OpenMP?

**OpenMP** stands for **Open Multi-Processing** and is an application programming interface (API) for parallel programming on multiprocessors.

### Components

1. **Compiler Directives**
   - Instructions to the compiler on how to parallelize code
   - Prefixed with `#pragma omp`
   - Easy to add to existing sequential code

2. **Library of Support Functions**
   - Runtime functions for thread management
   - Query functions to get thread information
   - Functions to set environment variables

3. **Language Support**
   - Works with Fortran, C, and C++
   - C + OpenMP is sufficient for multiprocessor programming
   - C + MPI + OpenMP for heterogeneous systems

### Evolution of OpenMP

- **1997:** First OpenMP standard released
- **2000:** OpenMP 2.0 (improvements and extensions)
- **2008:** OpenMP 3.0 (task parallelism)
- **2013:** OpenMP 4.0 (accelerators, vectorization)
- **2018:** OpenMP 5.0 (modern features)
- **Current:** Widely supported in GCC, Intel ICC, Clang

### Why Use OpenMP?

- Simpler than manual thread management
- Compiler handles low-level details
- Easy to incrementally parallelize code
- Good performance on shared-memory systems
- Portable across different hardware platforms

---

## Fork/Join Parallelism

### Execution Model

```
Time →

Master Thread (sequential code)
     ↓
   FORK (create additional threads)
     ├─→ Worker Thread 1
     ├─→ Worker Thread 2
     ├─→ Worker Thread 3
     ├─→ Worker Thread 4
     │ (parallel region - all threads execute same code)
     ├─←─ Worker Thread 1
     ├─←─ Worker Thread 2
     ├─←─ Worker Thread 3
     ├─←─ Worker Thread 4
   JOIN (threads synchronize)
     ↓
Master Thread (sequential code)
```

### Phases

1. **Initialization**
   - Only master thread is active
   - Master thread executes sequential code
   - Initialization and I/O typically done here

2. **Fork Phase**
   - Master thread creates (or awakens) additional threads
   - Threads begin executing parallel code region
   - All threads execute the same code block (SPMD model)

3. **Parallel Phase**
   - Multiple threads work concurrently
   - Each thread has its own program counter
   - Shared variables accessible to all threads

4. **Join Phase**
   - At the end of parallel code, threads synchronize
   - Worker threads are destroyed (or suspended)
   - Synchronization point (implicit barrier)
   - Control returns to master thread

5. **Finalization**
   - Only master thread is active
   - Results are gathered and output
   - Sequential code continues

### Important Property
- **Initial State:** Only master thread active
- **Final State:** Only master thread active
- **Dynamic:** Thread count changes during execution (unlike MPI)

---

## Shared-Memory vs. Message-Passing Model

### Key Differences

#### Programming Approach

**Shared-Memory Model:**
- Execute and profile sequential program
- Incrementally make it parallel
- Stop when further effort not warranted
- Gradual transformation process

**Message-Passing Model (MPI):**
- Sequential-to-parallel transformation requires major effort
- Transformation done in one giant step (not incremental)
- All data passing must be explicit
- Requires redesign of algorithm

#### Thread Lifecycle

**Shared-Memory Model:**
- Only one active thread at start and finish of program
- Changes dynamically during execution (fork/join)
- Simpler synchronization

**Message-Passing Model:**
- All processes active throughout entire execution of program
- Static number of processes
- Complex synchronization mechanisms

#### Advantages of Each

**Shared-Memory:**
- Easier for programmers familiar with sequential code
- Incremental parallelization possible
- Less code rewriting required
- Natural for problems with shared data

**Message-Passing:**
- Explicit data locality
- Clear communication patterns
- Portable to distributed systems
- Easier to debug (explicit communication)

---

## Parallel for Loops

### Sequential Loop Example

```c
for (i = 0; i < 10; i++)
    b[i] = i;
```

### Problem
- Sequential loop takes time proportional to 10
- Only one iteration executes at a time
- No parallelism exploited

### Solution: Data Parallelism
- Many C programs express data-parallel operations as for loops
- Multiple iterations can execute independently
- Each iteration doesn't depend on other iterations
- Perfect candidate for parallelization

### OpenMP Makes It Easy
- OpenMP provides a simple way to indicate parallelizable loops
- Programmer just adds a pragma directive
- Compiler generates threading code automatically
- Handles workload distribution among threads

---

## Pragmas

### Definition
- **Pragma** stands for "pragmatic information"
- A compiler directive in C or C++
- Way for the programmer to communicate with the compiler
- Compiler can ignore it if not supported (graceful degradation)

### Syntax

```c
#pragma omp <rest of pragma>
```

### OpenMP Pragma Structure

```c
#pragma omp <directive> [clauses]
{
    // Code affected by the directive
}
```

### Advantages
- Doesn't change the sequential code
- Can be added gradually
- Portable across compilers (most support OpenMP)
- Non-intrusive to code structure

---

## Parallel for Pragma

### Format

```c
#pragma omp parallel for
for (i = 0; i < 10; i++)
    b[i] = i;
```

### What It Does
- Tells compiler to parallelize the following loop
- Loop iterations are divided among threads
- Each thread executes a subset of iterations
- Automatic workload distribution

### Compiler Verification
- Compiler must verify the runtime system will have information needed to schedule loop iterations
- For example:
  - Loop bounds must be computable at runtime
  - Loop must be structured (no break statements affecting loop)
  - No data dependencies between iterations (or dependencies are explicit)

### Execution
1. Compiler inserts fork code
2. Multiple threads created
3. Each thread executes a subset of iterations
4. Implicit join/barrier at end of loop
5. Single thread continues

### Example with Thread Count

```c
#pragma omp parallel for num_threads(4)
for (i = 0; i < 1000; i++)
    c[i] = a[i] + b[i];
```

- Creates 4 threads
- Divides 1000 iterations among 4 threads (~250 per thread)
- Each thread computes its subset of results

---

## Execution Context

### Definition
**Execution context** is the address space containing all of the variables a thread may access.

### Scope of a Thread
Every thread has its own execution context containing:

1. **Static Variables**
   - Global variables
   - Static local variables
   - Shared across threads (careful with concurrent access)

2. **Dynamically Allocated Data Structures**
   - Heap allocations via malloc/new
   - Shared among threads if pointer is shared
   - Each thread can allocate its own heap memory

3. **Variables on the Runtime Stack**
   - Local variables within functions
   - Automatic variables
   - Unique to each thread
   - Different memory location for each thread

### Implications

#### Shared Variables
- Global variables are shared by default
- Require synchronization if accessed concurrently
- Visible to all threads

#### Private Variables
- Local function variables (stack variables)
- Automatically private to each thread
- No synchronization needed
- Can't affect other threads

### Execution Context Example

```c
int global_var = 10;        // Shared by all threads

void function() {
    int local_var = 5;      // Private to each thread
    static int s = 0;       // Shared (static storage)
    
    #pragma omp parallel
    {
        // All threads see the same global_var
        // Each thread has its own local_var
        // All threads see the same s
    }
}
```

---

## Thread Management Functions

### Getting Thread Information

```c
int omp_get_thread_num(void)
```
- Returns the ID of the current thread (0 to num_threads-1)
- Thread 0 is the master thread

```c
int omp_get_num_threads(void)
```
- Returns the number of threads in the current team
- Returns 1 if called outside parallel region

```c
int omp_get_max_threads(void)
```
- Returns the maximum number of threads available

### Setting Thread Count

```c
void omp_set_num_threads(int num)
```
- Sets the number of threads for subsequent parallel regions
- Must be called in sequential part of program
- Before parallel regions

### Environment Variables

```bash
# Set via shell environment
export OMP_NUM_THREADS=4
```

---

## Summary

### Key Takeaways

1. **Shared-Memory Programming**
   - All processors access same memory
   - Simple programming model
   - Synchronization through shared variables

2. **OpenMP**
   - Easy-to-use API for parallel programming
   - Compiler directives (pragmas)
   - Works with C, C++, Fortran

3. **Fork/Join Model**
   - Dynamic thread creation/destruction
   - Incremental parallelization possible
   - Only master active initially/finally

4. **Parallel for Loops**
   - Common pattern for data parallelism
   - OpenMP handles workload distribution
   - Compiler generates threading code

5. **Execution Context**
   - Global variables shared
   - Local variables private
   - Static variables shared

### Next Steps
- Learn data-sharing clauses (private, shared, reduction)
- Understand synchronization primitives
- Optimize for specific architectures
- Study performance considerations

---

## References

- OpenMP Official Documentation: https://www.openmp.org/
- GCC OpenMP Support: https://gcc.gnu.org/projects/gomp/
- LLVM OpenMP: https://openmp.llvm.org/
- Course Website: https://www.cs.trincoll.edu/~pyoon/cpsc375/
