# Lecture 24: The Sieve of Eratosthenes Implementation

**Course:** CPSC 375 High-Performance Computing  
**Topic:** Parallel Implementation of Prime Number Algorithm

---

## Block Decomposition Macros

### Overview

These macros efficiently calculate block boundaries and ownership for block-decomposed problems.

### Macro Definitions

```c
#define BLOCK_LOW(id, p, n) \
    ((id) * (n) / (p))

#define BLOCK_HIGH(id, p, n) \
    (BLOCK_LOW((id)+1, p, n) - 1)

#define BLOCK_SIZE(id, p, n) \
    (BLOCK_LOW((id)+1, p, n) - BLOCK_LOW((id), p, n))

#define BLOCK_OWNER(index, p, n) \
    (((p) * (index) + 1) - 1) / (n))
```

### Macro Explanations

#### BLOCK_LOW(id, p, n)

Returns the lowest index assigned to process `id`.

**Formula:** `id * n / p`

**Example:**
```
n = 100, p = 4
Process 0: BLOCK_LOW(0, 4, 100) = 0 * 100 / 4 = 0
Process 1: BLOCK_LOW(1, 4, 100) = 1 * 100 / 4 = 25
Process 2: BLOCK_LOW(2, 4, 100) = 2 * 100 / 4 = 50
Process 3: BLOCK_LOW(3, 4, 100) = 3 * 100 / 4 = 75
```

**Interpretation:** Process i starts at index `i*n/p`

#### BLOCK_HIGH(id, p, n)

Returns the highest index assigned to process `id`.

**Formula:** `BLOCK_LOW(id+1, p, n) - 1`

**Reasoning:** Process i's last element is one before process i+1's first

**Example:**
```
n = 100, p = 4
Process 0: BLOCK_HIGH(0, 4, 100) = BLOCK_LOW(1, 4, 100) - 1 = 25 - 1 = 24
Process 1: BLOCK_HIGH(1, 4, 100) = BLOCK_LOW(2, 4, 100) - 1 = 50 - 1 = 49
Process 2: BLOCK_HIGH(2, 4, 100) = BLOCK_LOW(3, 4, 100) - 1 = 75 - 1 = 74
Process 3: BLOCK_HIGH(3, 4, 100) = BLOCK_LOW(4, 4, 100) - 1 = 100 - 1 = 99
```

#### BLOCK_SIZE(id, p, n)

Returns the number of elements assigned to process `id`.

**Formula:** `BLOCK_LOW(id+1, p, n) - BLOCK_LOW(id, p, n)`

**Example:**
```
n = 100, p = 4
Process 0: BLOCK_SIZE(0, 4, 100) = 25 - 0 = 25
Process 1: BLOCK_SIZE(1, 4, 100) = 50 - 25 = 25
Process 2: BLOCK_SIZE(2, 4, 100) = 75 - 50 = 25
Process 3: BLOCK_SIZE(3, 4, 100) = 100 - 75 = 25
```

#### BLOCK_OWNER(index, p, n)

Returns which process owns the given `index`.

**Formula:** `(((p) * (index) + 1) - 1) / (n)` = `((p * index) / n)`

**Example:**
```
n = 100, p = 4
BLOCK_OWNER(10, 4, 100) = (4 * 10) / 100 = 0 (Process 0)
BLOCK_OWNER(40, 4, 100) = (4 * 40) / 100 = 1 (Process 1)
BLOCK_OWNER(60, 4, 100) = (4 * 60) / 100 = 2 (Process 2)
BLOCK_OWNER(90, 4, 100) = (4 * 90) / 100 = 3 (Process 3)
```

**Usage:** Find which process to send marked element information

---

## Implementation Part 1: Initialization

### Code Structure

```c
#define MIN(a, b) ((a) < (b) ? (a) : (b))

int main(int argc, char *argv[]) {
    // 1. Initialize MPI
    MPI_Init(&argc, &argv);
    
    // 2. Barrier for timing
    MPI_Barrier(MPI_COMM_WORLD);
    
    // 3. Start timer
    double elapsed_time = -MPI_Wtime();
    
    // 4. Get process rank and size
    int id;
    int p;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    
    // 5. Check command-line arguments
    if (argc != 2) {
        if (!id) printf("Command line: %s <m>\n", argv[0]);
        MPI_Finalize();
        exit(1);
    }
    
    // ... rest of implementation ...
}
```

### Step-by-Step Explanation

#### 1. MPI Initialization

```c
MPI_Init(&argc, &argv);
```
- Initializes MPI environment
- Must be first MPI call
- Passes command-line arguments

#### 2. Barrier for Timing

```c
MPI_Barrier(MPI_COMM_WORLD);
```
- Synchronizes all processes
- Ensures all start timing together
- Important for accurate measurements
- All processes wait here

#### 3. Start Timer

```c
double elapsed_time = -MPI_Wtime();
```
- `MPI_Wtime()` returns current wall-clock time
- Negating the initial value allows adding final time
- Simpler than tracking start and end separately

#### 4. Get Process Information

```c
MPI_Comm_rank(MPI_COMM_WORLD, &id);
MPI_Comm_size(MPI_COMM_WORLD, &p);
```
- `id`: rank of current process (0 to p-1)
- `p`: total number of processes

#### 5. Argument Validation

```c
if (argc != 2) {
    if (!id) printf("Command line: %s <m>\n", argv[0]);
    MPI_Finalize();
    exit(1);
}
```
- Checks for correct number of arguments
- Only process 0 prints (avoids duplicate messages)
- Clean exit with MPI_Finalize

---

## Implementation Part 2: Setup

### Calculate Array Bounds

```c
// Get the upper limit for prime search
int n = atoi(argv[1]);

// Calculate block boundaries for this process
int low_value = 2 + BLOCK_LOW(id, p, n - 1);
int high_value = 2 + BLOCK_HIGH(id, p, n - 1);
int size = BLOCK_SIZE(id, p, n - 1);

// Calculate size of process 0's block (for later use)
int proc0_size = (n - 1) / p;

// Allocate array
char *marked = (char *)malloc(size);

if (marked == NULL) {
    printf("Cannot allocate enough memory\n");
    MPI_Finalize();
    exit(1);
}

// Initialize all elements as unmarked (0 = not marked, i.e., potentially prime)
for (int i = 0; i < size; i++)
    marked[i] = 0;
```

### Explanation

#### Why `2 + BLOCK_LOW(id, p, n-1)`?

- We're looking for primes between 2 and n
- Array indices: 0 to n-2 represent numbers 2 to n
- `BLOCK_LOW` returns index from 0 to n-2
- Add 2 to get actual number (2 represents index 0)

**Mapping:**
```
Index 0 → Number 2
Index 1 → Number 3
Index 2 → Number 4
...
Index i → Number i + 2
```

#### Block Assignment Example

**n = 31, p = 4:**
```
Process 0: low_value = 2 + 0 = 2,  high_value = 2 + 7 = 9
           size = 8
           marked array: [2, 3, 4, 5, 6, 7, 8, 9]

Process 1: low_value = 2 + 8 = 10, high_value = 2 + 15 = 17
           size = 8
           marked array: [10, 11, 12, 13, 14, 15, 16, 17]

Process 2: low_value = 2 + 16 = 18, high_value = 2 + 23 = 25
           size = 8
           marked array: [18, 19, 20, 21, 22, 23, 24, 25]

Process 3: low_value = 2 + 24 = 26, high_value = 2 + 29 = 31
           size = 6
           marked array: [26, 27, 28, 29, 30, 31]
```

#### Memory Allocation

```c
char *marked = (char *)malloc(size);
```
- Allocates `size` bytes for marked array
- Each element: 1 byte (0 = unmarked, 1 = marked)
- Check for NULL return (memory allocation failure)

---

## Implementation Part 3: Main Sieving Loop

### Code

```c
// Initialize prime search
if (!id)
    index = 0;  // Only process 0 tracks index

int prime = 2;  // First prime

// Repeat until prime squared exceeds n
do {
    // Step 1: Determine where to start marking in local array
    if (prime * prime > low_value)
        first = prime * prime - low_value;  // Start from prime²
    else {
        // Calculate first multiple of prime >= low_value
        if (!(low_value % prime))
            first = 0;  // low_value is divisible by prime
        else
            first = prime - (low_value % prime);
    }
    
    // Step 2: Mark all multiples of prime in local array
    for (i = first; i < size; i += prime)
        marked[i] = 1;  // Mark as composite
    
    // Step 3: Process 0 finds next prime
    if (!id) {
        // Find next unmarked number (next prime)
        while (marked[++index]);
        prime = index + 2;  // Convert index back to number
    }
    
    // Step 4: Broadcast new prime to all processes
    MPI_Bcast(&prime, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
} while (prime * prime <= n);
```

### Step-by-Step Breakdown

#### Step 1: Calculate First Multiple of Prime

**Goal:** Find first multiple of `prime` >= `low_value`

**Case 1: prime² > low_value**
```c
first = prime * prime - low_value;
```
- Start marking from prime²
- Example: prime = 5, low_value = 2
  - first = 25 - 2 = 23 (mark elements starting at index 23)

**Case 2: prime² ≤ low_value**
```c
if (!(low_value % prime))
    first = 0;              // low_value divisible by prime
else
    first = prime - (low_value % prime);  // Next multiple
```

**Example:**
```
prime = 7, low_value = 50
50 % 7 = 1 (not divisible)
next_multiple = 7 - 1 = 6
So mark from index 6 in local array
```

#### Step 2: Mark Multiples

```c
for (i = first; i < size; i += prime)
    marked[i] = 1;
```
- Mark every `prime`-th element
- Starting from `first`
- Efficiently marks all multiples

#### Step 3: Find Next Prime (Process 0 Only)

```c
if (!id) {
    while (marked[++index]);  // Skip marked elements
    prime = index + 2;         // Convert to actual number
}
```

**Example:**
```
marked array on Process 0: [0, 0, 0, 1, 0, 1, 1, 1]
                           (indices: 0, 1, 2, 3, 4, 5, 6, 7)

Starting with index = 0:
- ++index = 1: marked[1] = 0 (unmarked) → Stop
- prime = 1 + 2 = 3

Next iteration:
- ++index = 2: marked[2] = 0 → Stop
- prime = 2 + 2 = 4
```

#### Step 4: Broadcast Prime

```c
MPI_Bcast(&prime, 1, MPI_INT, 0, MPI_COMM_WORLD);
```
- Process 0 sends new prime to all
- All processes use same prime for marking
- Ensures consistency

#### Loop Termination

```c
while (prime * prime <= n)
```
- Stops when prime² > n
- No point marking further (composites are covered)

---

## Implementation Part 4: Counting and Finalization

### Counting Primes

```c
// Count unmarked elements (primes) in local array
int count = 0;
for (i = 0; i < size; i++)
    if (!marked[i])
        count++;

// Reduce to get global prime count
int global_count;
MPI_Reduce(&count, &global_count, 1, MPI_INT, MPI_SUM, 0,
          MPI_COMM_WORLD);

// Stop timing
elapsed_time += MPI_Wtime();

// Output results (only process 0)
if (!id) {
    printf("%d primes are <= %d\n", global_count, n);
    printf("Total elapsed time: %10.6f\n", elapsed_time);
}

// Cleanup
MPI_Finalize();
return 0;
```

### Explanation

#### Local Counting

```c
int count = 0;
for (i = 0; i < size; i++)
    if (!marked[i])
        count++;
```
- Count unmarked elements in local array
- Only counts this process's portion

#### Global Reduction

```c
MPI_Reduce(&count, &global_count, 1, MPI_INT, MPI_SUM, 0,
          MPI_COMM_WORLD);
```
- Sums all local counts
- Result sent to process 0
- Only process 0 receives result

#### Timing

```c
elapsed_time += MPI_Wtime();
```
- Adds final time to negated initial value
- Result: Total elapsed time
- More accurate with barrier at start

#### Output

```c
if (!id) {
    printf("%d primes are <= %d\n", global_count, n);
    printf("Total elapsed time: %10.6f\n", elapsed_time);
}
```
- Only process 0 prints
- Avoids duplicate messages from p processes

---

## Optimizations Discussed

### 1. Skip Even Numbers

**Strategy:** Only store odd numbers after 2

**Benefits:**
- 50% memory reduction
- 2x fewer operations
- Still finds all primes

**Implementation:** Use different index mapping

### 2. Eliminate Broadcast

**Strategy:** Each process computes next prime locally

**Benefits:**
- Eliminates MPI_Bcast call
- Processes work independently
- No broadcast overhead

**Cost:**
- Redundant computation (√n operations × p processes)
- Usually worth it

### 3. Cache Optimization

**Strategy:** Reorganize loops for sequential memory access

**Benefits:**
- Better cache hit rate
- 10-100x performance improvement
- Critical for large n

**Implementation:** Loop over primes in outer loop, array index in inner loop

---

## Performance Considerations

### Scalability

- **Strong scaling:** Problem size fixed, vary p
  - Communication overhead grows with p
  - Eventually communication dominates
  - Sweet spot depends on n and p

- **Weak scaling:** Problem size grows with p
  - Keep n/p constant
  - Communication overhead remains constant
  - Better scalability than strong scaling

### Load Balancing

- Block decomposition distributes work evenly initially
- Marked elements not distributed evenly
- Late in algorithm, some processors have more work
- Overall effect usually acceptable

### Network Topology

- Broadcast uses tree structure
- Performance depends on network
- Modern networks: logarithmic overhead
- Efficient for distributed memory

---

## Summary

### Implementation Key Points

1. **Block Decomposition**
   - Use BLOCK_* macros for efficient indexing
   - Low_value, high_value for block boundaries
   - Size for array allocation

2. **Main Loop**
   - Calculate first multiple of prime
   - Mark multiples in local array
   - Process 0 finds next prime
   - Broadcast to all processes

3. **Counting and Output**
   - Local count of unmarked elements
   - Global reduction for total count
   - Output from process 0 only

4. **Timing**
   - Barrier before starting timer
   - Accurate measurement of parallel portion
   - Useful for performance analysis

### Lessons Learned

1. **Algorithm Selection** Matters
   - Block better than cyclic for this problem
   - Affects scalability and performance

2. **Communication Patterns**
   - Broadcast every iteration
   - Can be bottleneck
   - Optimization reduces cost

3. **Cache Matters**
   - Memory access patterns critical
   - Can dominate execution time
   - Reorganization yields big gains

4. **Optimization Process**
   - Profile before optimizing
   - Focus on actual bottlenecks
   - Validate improvements

---

## References

- Course Website: https://www.cs.trincoll.edu/~pyoon/cpsc375/
- Pacheco, P. S. (2011). An Introduction to Parallel Programming
- MPI Forum: https://www.mpi-forum.org/
