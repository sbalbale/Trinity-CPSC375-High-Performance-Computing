# Lecture 21: Collective Communication in MPI

**Course:** CPSC 375 High-Performance Computing  
**Topic:** Collective Communication Patterns and Functions

---

## Collective Communication Overview

### Definition

**Collective communication** involves multiple processes coordinating to complete a communication operation.

### Key Characteristics

1. **Synchronous Nature**
   - All processes in communicator must participate
   - Implicit synchronization barrier
   - Processes wait for all others

2. **Single Function Call**
   - One MPI function call initiates communication
   - Underlying implementation handles complexity
   - Optimized by MPI runtime

3. **Standard Patterns**
   - Broadcast: One to all
   - Reduce: All to one (with operation)
   - Allreduce: All to all with operation
   - Scatter: One distributes to all
   - Gather: All contribute to one
   - Barrier: Synchronization point

---

## Collective Communication Patterns

### Pattern 1: Broadcast

```
         Process 0
           (Data)
             │
      ┌──────┼──────┐
      │      │      │
      ▼      ▼      ▼
   Proc 1  Proc 2  Proc 3
   (copy) (copy) (copy)
```

**Purpose:** Send data from one process to all processes
- Process 0 sends data
- All other processes receive the same copy
- All processes get identical data

### Pattern 2: Reduce

```
   Proc 0   Proc 1   Proc 2   Proc 3
   (10)     (20)     (30)     (40)
      │      │      │      │
      └──────┼──────┼──────┘
             │  (operation: SUM)
             ▼
           Process 0
           (Result: 100)
```

**Purpose:** Combine data from all processes
- Each process contributes a value
- Reduction operation applied (sum, max, min, etc.)
- Result sent to designated process
- Other processes don't receive result

### Pattern 3: Allreduce

```
   Proc 0   Proc 1   Proc 2   Proc 3
   (10)     (20)     (30)     (40)
      │      │      │      │
      └──────┼──────┼──────┘
             │  (operation: SUM)
      ┌──────┼──────┬──────┐
      │      │      │      │
      ▼      ▼      ▼      ▼
   Proc 0  Proc 1  Proc 2  Proc 3
   (100)  (100)   (100)   (100)
```

**Purpose:** Combine data and send result to all
- Same as reduce but result distributed to all
- Every process gets the combined result
- Useful for global reductions

### Pattern 4: Scatter

```
          Process 0
        [1,2,3,4]
         ││││││
    ┌────┼┼┼┼────┐
    │    ││││    │
    ▼    ▼▼▼▼    ▼
  Proc 0 Proc 1 Proc 2 Proc 3
  [1,2]  [3,4]  [5,6]  [7,8]
```

**Purpose:** Distribute data from one process to all
- Process 0 has array of data
- Array divided into chunks
- Each process receives its chunk
- Opposite of gather

### Pattern 5: Gather

```
  Proc 0  Proc 1  Proc 2  Proc 3
  [1,2]   [3,4]   [5,6]   [7,8]
    │       │       │       │
    └───────┼───────┼───────┘
            │
          Process 0
        [1,2,3,4,5,6,7,8]
```

**Purpose:** Collect data from all processes to one
- Each process contributes its data
- Process 0 assembles complete data
- Opposite of scatter

### Pattern 6: Barrier

```
   Proc 0  Proc 1  Proc 2  Proc 3
     │       │       │       │
  ←→ │       │       │ ←→    │
     │       │       │       │
     └───────┴───────┴───────┘
          (Synchronization Point)
     ┌───────┬───────┬───────┐
     │       │       │       │
     ▼       ▼       ▼       ▼
  Proc 0  Proc 1  Proc 2  Proc 3
```

**Purpose:** Synchronization point
- All processes wait until all reach barrier
- No process continues past barrier until all arrive
- Used for timing and ensuring order

---

## Tree-Structured Communication

### Concept

For efficient collective operations on large numbers of processes, tree structures are used.

### Example: Broadcast with Tree

```
                Process 0
               (broadcasts)
                   │
        ┌──────────┼──────────┐
        │          │          │
     Proc 1     Proc 2     Proc 3
        │          │          │
   ┌────┼─────┐ ┌──┴──┐   ┌───┼────┐
   │    │     │ │     │   │   │    │
Proc 4 5 6   7 8 9 10 11 12 13 14 15
(Binary tree structure)
```

### Advantages

1. **Logarithmic Depth**
   - Broadcast takes O(log P) time with tree
   - Linear with naive implementation

2. **Parallel Communication**
   - Multiple transfers happen simultaneously
   - Better network utilization
   - Avoids bottlenecks

3. **Scalability**
   - Performance improves with more processes
   - Works well for large-scale systems

### Implementation Details

The MPI implementation chooses the optimal tree structure based on:
- Number of processes
- Hardware topology
- Network characteristics

Programmers don't need to implement tree logic—MPI handles it automatically.

---

## MPI_Reduce Function

### Function Signature

```c
int MPI_Reduce(
    void* in_data_p,      /* in */
    void* out_data_p,     /* out */
    int count,            /* in */
    MPI_Datatype datatype,/* in */
    MPI_Op operator,      /* in */
    int dest_proc,        /* in */
    MPI_Comm comm         /* in */
);
```

### Purpose

Allows every processor to contribute data which is combined using a mathematical operation, with the final result delivered to one specific processor.

### Parameters

1. **in_data_p:** Pointer to input data
   - Data to contribute
   - Must point to valid data
   
2. **out_data_p:** Pointer to output buffer
   - Where result is stored
   - Only used on dest_proc (but all must provide)
   
3. **count:** Number of elements
   - For single value: 1
   - For arrays: size of array
   
4. **datatype:** MPI data type
   - `MPI_INT`, `MPI_DOUBLE`, `MPI_FLOAT`, etc.
   - Must match the data
   
5. **operator:** Reduction operation
   - `MPI_SUM`, `MPI_MAX`, `MPI_MIN`, etc.
   
6. **dest_proc:** Destination process rank
   - Which process gets the result
   - Usually 0
   
7. **comm:** Communicator
   - `MPI_COMM_WORLD` for all processes

### Examples

**Example 1: Sum reduction**
```c
int local_int = 5;
int total_int;

MPI_Reduce(&local_int, &total_int, 1, MPI_INT, MPI_SUM, 0, 
           MPI_COMM_WORLD);

// Only process 0 has total_int (sum of all local_int values)
```

**Example 2: Array reduction**
```c
double local_x[N];
double sum[N];

MPI_Reduce(local_x, sum, N, MPI_DOUBLE, MPI_SUM, 0, 
           MPI_COMM_WORLD);

// Only process 0 has sum array (element-wise sum)
```

### Predefined Reduction Operators

| Operator | Meaning | Example |
|----------|---------|---------|
| `MPI_SUM` | Addition | 10 + 20 + 30 = 60 |
| `MPI_PROD` | Multiplication | 2 × 3 × 4 = 24 |
| `MPI_MAX` | Maximum | max(10, 20, 30) = 30 |
| `MPI_MIN` | Minimum | min(10, 20, 30) = 10 |
| `MPI_LAND` | Logical AND | true AND false = false |
| `MPI_BAND` | Bitwise AND | 5 & 3 = 1 |
| `MPI_LOR` | Logical OR | true OR false = true |
| `MPI_BOR` | Bitwise OR | 5 \| 3 = 7 |
| `MPI_LXOR` | Logical XOR | true XOR false = true |
| `MPI_BXOR` | Bitwise XOR | 5 ^ 3 = 6 |
| `MPI_MAXLOC` | Max and location | Returns max value and its index |
| `MPI_MINLOC` | Min and location | Returns min value and its index |

### Properties

1. **Associativity Required**
   - Operation must be associative
   - Order of evaluation doesn't matter
   - (a ⊕ b) ⊕ c = a ⊕ (b ⊕ c)

2. **Commutativity Not Required**
   - MPI doesn't require operations to be commutative
   - But results should be well-defined
   - Floating-point arithmetic issues may arise

---

## Collective vs. Point-to-Point Communications

### Key Differences and Rules

#### Rule 1: All Processes Must Participate

```c
// CORRECT
MPI_Reduce(&local_value, &global_sum, 1, MPI_INT, MPI_SUM, 0, 
           MPI_COMM_WORLD);

// INCORRECT - trying to match collective with point-to-point
// Process 0
MPI_Reduce(&local_value, &global_sum, 1, MPI_INT, MPI_SUM, 0, 
           MPI_COMM_WORLD);

// Process 1
MPI_Recv(&data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, 
         MPI_STATUS_IGNORE);
// ERROR: Mismatched operations lead to deadlock or crash
```

**Requirement:** All processes in communicator must call the same collective function.

#### Rule 2: Compatible Arguments

```c
// CORRECT - all processes use same operator and destination
MPI_Reduce(&val, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

// INCORRECT - different dest_proc on different processes
// Process 0
MPI_Reduce(&val, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

// Process 1
MPI_Reduce(&val, &sum, 1, MPI_INT, MPI_SUM, 1, MPI_COMM_WORLD);
// ERROR: Processes disagree on where to send result
```

**Requirement:** Arguments passed by each process must be "compatible."
- Same datatype
- Same operation
- Same destination (for reduce)

#### Rule 3: Output Buffer Must Be Provided

```c
// Process 0 (receives result)
MPI_Reduce(&local_int, &total_int, 1, MPI_INT, MPI_SUM, 0, 
           MPI_COMM_WORLD);
// total_int contains sum

// Process 1 (doesn't use output buffer)
MPI_Reduce(&local_int, NULL, 1, MPI_INT, MPI_SUM, 0, 
           MPI_COMM_WORLD);
// NULL is acceptable for non-receiving processes

// BEST PRACTICE: All processes provide buffer
MPI_Reduce(&local_int, &total_int, 1, MPI_INT, MPI_SUM, 0, 
           MPI_COMM_WORLD);
// Output buffer only used on dest_proc, but all must allocate
```

**Requirement:** 
- out_data_p is only used on dest_proc
- However, all processes still need to pass valid argument
- Can pass NULL if not needed, but typically allocate buffer

#### Rule 4: Matching Semantics

**Point-to-Point Matching:**
```c
// Process 0
MPI_Send(&data, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);

// Process 1
MPI_Recv(&buffer, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, 
         &status);
// Matched by: source process (0), destination (1), and tag
```

**Collective Matching:**
```c
// All processes must agree
MPI_Reduce(&value, &result, 1, MPI_INT, MPI_SUM, 0, 
           MPI_COMM_WORLD);
// Matched by: same communicator, function, and compatible args
```

---

## Implementation Strategies

### Naive Broadcast (Linear)

```
Process 0                        
  │
  ├─→ P1 (send)
  │    (wait for ack)
  ├─→ P2 (send)
  │    (wait for ack)
  ├─→ P3 (send)
  │    (wait for ack)
  └─→ P4 (send)
```

**Time Complexity:** O(P) where P is number of processes
**Problem:** Linear time, bottleneck at process 0

### Tree Broadcast (Optimized)

```
Process 0
  │
  ├─→ P1 ─┐
  │       ├─→ P4, P5
  ├─→ P2 ─┤
  │       └─→ P6, P7
  └─→ P3
```

**Time Complexity:** O(log P)
**Advantage:** Parallel communication, much faster

### Reduce with Tree

```
       Sum
      ╱   ╲
   Sum     Sum
   ╱ ╲    ╱  ╲
  P0  P1 P2  P3
```

Multiple processes can contribute in parallel, reducing total time.

---

## Alternative Communication Patterns

### All-to-All Pattern

```
Proc 0  Proc 1  Proc 2  Proc 3
  │      │      │      │
  └──┬───┼──┬───┼──┬───┘
     │   │  │   │  │
  ┌──┼───┼──┼───┼──┼──┐
  │  │   │  │   │  │  │
  ▼  ▼   ▼  ▼   ▼  ▼  ▼
All processes send to and receive from all
```

**Purpose:** Total exchange of data
**MPI Function:** `MPI_Alltoall()`

---

## Summary

### Key Concepts

1. **Collective Communication**
   - All processes participate
   - Single function call
   - Optimized implementations

2. **Common Patterns**
   - Broadcast: One to all
   - Reduce: All to one (with operation)
   - Allreduce: All to all (with operation)
   - Scatter/Gather: Distribute/Collect
   - Barrier: Synchronization

3. **MPI_Reduce Details**
   - Combines data from all processes
   - Applies mathematical operation
   - Sends result to one process

4. **Important Rules**
   - All processes must participate
   - Arguments must be compatible
   - Output buffer must be provided
   - Deadlocks occur with violations

5. **Implementation**
   - Tree structures for efficiency
   - Logarithmic time complexity
   - Optimized by MPI runtime

### Common Usage Pattern

```c
// All processes contribute
int my_value = calculate();

// Reduce to get sum
int global_sum;
MPI_Reduce(&my_value, &global_sum, 1, MPI_INT, MPI_SUM, 0, 
           MPI_COMM_WORLD);

// Only process 0 has global_sum
if (my_rank == 0) {
    printf("Global sum: %d\n", global_sum);
}
```

### Performance Considerations

1. **Overhead**
   - Collective operations have startup overhead
   - Worth it for large data quantities
   - For small data, point-to-point might be faster

2. **Scalability**
   - Tree structures scale well
   - Logarithmic complexity grows slowly
   - Works efficiently to thousands of processes

3. **Load Balancing**
   - Collectives ensure all processes participate
   - No process is idle
   - All finish together

---

## References

- MPI Standard: https://www.mpi-forum.org/
- OpenMPI Documentation: https://www.open-mpi.org/doc/
- Gropp, W., Lusk, E., & Skjellum, A. (1999). Using MPI
- Course Website: https://www.cs.trincoll.edu/~pyoon/cpsc375/
