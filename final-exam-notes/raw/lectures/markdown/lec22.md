# Lecture 22: Derived Types and Barriers in MPI

**Course:** CPSC 375 High-Performance Computing  
**Topic:** Custom Data Types and Synchronization

---

## The Problem: Broadcasting Multiple Values

### Inefficient Approach

Naive approach: Send each value separately

```c
void Get_input(
    int my_rank,     /* in */
    int comm_sz,     /* in */
    double* a_p,     /* out */
    double* b_p,     /* out */
    int* n_p         /* out */)
{
    int dest;
    
    if (my_rank == 0) {
        printf("Enter a, b, and n\n");
        scanf("%lf %lf %d", a_p, b_p, n_p);
        
        // Send each value individually
        for (dest = 1; dest < comm_sz; dest++) {
            MPI_Send(a_p, 1, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
            MPI_Send(b_p, 1, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
            MPI_Send(n_p, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
        }
    } else {
        // Receive each value individually
        MPI_Recv(a_p, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, 
                 MPI_STATUS_IGNORE);
        MPI_Recv(b_p, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, 
                 MPI_STATUS_IGNORE);
        MPI_Recv(n_p, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, 
                 MPI_STATUS_IGNORE);
    }
}
```

### Problems with This Approach

1. **Multiple Messages**
   - 3 separate send operations per process
   - 3 separate receive operations per process
   - Message overhead multiplied
   - Scales poorly with number of values

2. **Inefficiency**
   - Each message has overhead (headers, routing, etc.)
   - Bundling data into single message reduces overhead
   - Better to combine and send once

3. **Example Overhead**
   - 4 processes × 3 messages = 12 messages total
   - 1000 processes × 3 messages = 3000 messages
   - Network congestion and latency issues

---

## Derived Datatypes

### Definition

**Derived datatype** is a representation of any collection of data items in memory.

Formally: A sequence of basic MPI data types with displacement for each.

### Purpose

A derived datatype consists of:
1. **Data type information** (MPI_INT, MPI_DOUBLE, etc.)
2. **Displacement information** (where in memory)

This allows MPI to:
- **Send:** Collect data items from memory before sending
- **Receive:** Distribute data items into correct memory locations when received

### Advantages

1. **Efficiency**
   - Single message contains multiple data items
   - Reduces message overhead
   - Better network utilization

2. **Convenience**
   - Bundle related data into single type
   - More readable code
   - Type checking by compiler

3. **Flexibility**
   - Define any memory layout
   - Handle complex structures
   - Support strided access patterns

---

## MPI_Type_create_struct

### Function for Building Derived Types

```c
int MPI_Type_create_struct(
    int count,                          /* in */
    int array_of_blocklengths[],       /* in */
    MPI_Aint array_of_displacements[], /* in */
    MPI_Datatype array_of_types[],     /* in */
    MPI_Datatype* new_type_p           /* out */
);
```

### Purpose

Builds a derived datatype from individual elements that have **different basic types**.

### Parameters

1. **count:** Number of members in the structure
   - 3 in our example (a, b, n)

2. **array_of_blocklengths:** Array of repetition counts
   - For each member, how many elements
   - Usually 1 for each field (single values)

3. **array_of_displacements:** Array of byte offsets
   - Memory address relative to base
   - Calculated from structure layout

4. **array_of_types:** Array of MPI data types
   - MPI_DOUBLE for a
   - MPI_DOUBLE for b
   - MPI_INT for n

5. **new_type_p:** Output parameter
   - Pointer to new MPI data type
   - Used in send/receive operations

### Example Structure

```c
struct {
    double a;    // offset 0
    double b;    // offset 8 (assuming 8 bytes for double)
    int n;       // offset 16
}
```

---

## MPI_Get_address

### Function for Finding Memory Addresses

```c
int MPI_Get_address(
    void* location_p,      /* in */
    MPI_Aint* address_p    /* out */
);
```

### Purpose

Returns the address of the memory location referenced by location_p.

### Return Type

Special type `MPI_Aint`:
- Integer type large enough to store an address
- Portable across different architectures
- Use this for all address calculations in MPI

### Usage in Derived Type Creation

```c
double *a_p, *b_p;
int *n_p;

MPI_Aint a_addr, b_addr, n_addr;

// Get addresses of each field
MPI_Get_address(a_p, &a_addr);
MPI_Get_address(b_p, &b_addr);
MPI_Get_address(n_p, &n_addr);

// Calculate relative displacements
MPI_Aint displacements[3];
displacements[0] = 0;           // First element at base
displacements[1] = b_addr - a_addr;  // Relative to a
displacements[2] = n_addr - a_addr;  // Relative to a
```

### Why Use MPI_Get_address?

1. **Portability**
   - Handles different compiler layouts
   - Correct on any architecture
   - Avoids hardcoding offsets

2. **Flexibility**
   - Works with any memory layout
   - Compiler padding handled automatically
   - Dynamic structures supported

3. **Correctness**
   - Ensures correct displacements
   - Avoids off-by-one errors
   - Works with optimization flags

---

## MPI_Type_commit

### Function to Finalize Derived Type

```c
int MPI_Type_commit(MPI_Datatype* new_mpi_t_p);
```

### Purpose

Allows the MPI implementation to optimize its internal representation of the datatype for use in communication functions.

### Why Is This Needed?

After creating a derived type with `MPI_Type_create_struct`:
- The type is defined but not ready
- MPI may optimize internal representation
- Must call commit before using in send/receive

### Usage

```c
MPI_Datatype input_mpi_t;

// Create the type
MPI_Type_create_struct(3, array_of_blocklengths,
                       array_of_displacements, array_of_types,
                       &input_mpi_t);

// Commit the type (prepare for use)
MPI_Type_commit(&input_mpi_t);

// Now can use in send/receive
MPI_Send(&data, 1, input_mpi_t, dest, tag, MPI_COMM_WORLD);
```

---

## MPI_Type_free

### Function to Release Type Resources

```c
int MPI_Type_free(MPI_Datatype* new_mpi_t_p);
```

### Purpose

When finished with a derived datatype, free any additional storage used.

### Usage

```c
// When done with the datatype
MPI_Type_free(&input_mpi_t);
```

### Importance

1. **Resource Management**
   - MPI allocates internal structures
   - Must be freed to avoid memory leaks

2. **Good Practice**
   - Call MPI_Type_free for each created type
   - Before MPI_Finalize

3. **Prevents Issues**
   - Memory leaks in long-running programs
   - Performance degradation over time

---

## Complete Example: Get Input with Derived Datatype

### Build the Derived Type

```c
void Build_mpi_type(
    double* a_p,     /* in */
    double* b_p,     /* in */
    int* n_p,        /* in */
    MPI_Datatype* input_mpi_t_p /* out */)
{
    // Define block lengths (each element repeated once)
    int array_of_blocklengths[3] = {1, 1, 1};
    
    // Define data types
    MPI_Datatype array_of_types[3] = {MPI_DOUBLE, 
                                      MPI_DOUBLE, 
                                      MPI_INT};
    
    // Get memory addresses
    MPI_Aint a_addr, b_addr, n_addr;
    MPI_Get_address(a_p, &a_addr);
    MPI_Get_address(b_p, &b_addr);
    MPI_Get_address(n_p, &n_addr);
    
    // Calculate displacements
    MPI_Aint array_of_displacements[3] = {0};
    array_of_displacements[1] = b_addr - a_addr;
    array_of_displacements[2] = n_addr - a_addr;
    
    // Create the derived type
    MPI_Type_create_struct(3, array_of_blocklengths,
                          array_of_displacements, 
                          array_of_types,
                          input_mpi_t_p);
    
    // Commit the type
    MPI_Type_commit(input_mpi_t_p);
}
```

### Use the Derived Type

```c
void Get_input_derived(
    int my_rank,          /* in */
    int comm_sz,          /* in */
    double* a_p,          /* out */
    double* b_p,          /* out */
    int* n_p,             /* out */
    MPI_Datatype input_mpi_t) /* in */
{
    if (my_rank == 0) {
        printf("Enter a, b, and n\n");
        scanf("%lf %lf %d", a_p, b_p, n_p);
        
        // Send all three values in one message
        for (int dest = 1; dest < comm_sz; dest++) {
            MPI_Send(a_p, 1, input_mpi_t, dest, 0, 
                     MPI_COMM_WORLD);
        }
    } else {
        // Receive all three values in one message
        MPI_Recv(a_p, 1, input_mpi_t, 0, 0, 
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}
```

### Main Program

```c
int main(int argc, char *argv[]) {
    int my_rank, comm_sz;
    double a, b;
    int n;
    MPI_Datatype input_mpi_t;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    
    // Build custom datatype
    Build_mpi_type(&a, &b, &n, &input_mpi_t);
    
    // Get input using derived type
    Get_input_derived(my_rank, comm_sz, &a, &b, &n, 
                     input_mpi_t);
    
    // ... rest of program ...
    
    // Clean up
    MPI_Type_free(&input_mpi_t);
    MPI_Finalize();
    
    return 0;
}
```

---

## Barriers in MPI

### MPI_Barrier Function

```c
int MPI_Barrier(MPI_Comm comm);
```

### Purpose

Creates a synchronization point where all processes wait until all have reached the barrier.

### Behavior

```
Process 0: ─────●────── (waits at barrier)
Process 1: ──●────────── (waits at barrier)
Process 2: ──────●────── (waits at barrier)
Process 3: ─●──────────── (waits at barrier)

                ▼ (All continue together)
Process 0: ─────●────→
Process 1: ──●────────→
Process 2: ──────●────→
Process 3: ─●─────────→
```

### Usage

```c
// Do some work
compute();

// Synchronize all processes
MPI_Barrier(MPI_COMM_WORLD);

// Continue (all processes have finished compute())
more_work();
```

### Common Uses

1. **Timing**
   - Ensure all processes reach same point
   - Measure wall-clock time accurately
   - All processes start/stop together

2. **Debugging**
   - Force specific execution order
   - Reproduce non-deterministic bugs
   - Isolate synchronization issues

3. **Load Balancing**
   - Ensure no process runs ahead excessively
   - Detect imbalanced workloads
   - Synchronize before next phase

### Example: Timing with Barrier

```c
// Synchronize all processes
MPI_Barrier(MPI_COMM_WORLD);

// Start timer
double start = MPI_Wtime();

// Do work
do_computation();

// Stop timer
double end = MPI_Wtime();

// Calculate elapsed time
double elapsed = end - start;

printf("Process %d elapsed time: %f seconds\n", 
       my_rank, elapsed);
```

### Performance Considerations

1. **Overhead**
   - Barrier synchronization adds latency
   - All processes wait for slowest
   - Unnecessary barriers hurt performance

2. **Load Imbalance**
   - If work not balanced, many processes wait
   - Visible as barrier time in profiling
   - Indicates need for better load distribution

3. **When to Use**
   - Only when synchronization actually needed
   - Before collective operations (implicit)
   - For timing and debugging
   - Avoid in performance-critical sections

---

## Advanced Datatypes

### Contiguous Type

```c
MPI_Type_contiguous(count, oldtype, &newtype);
```

Used for arrays of basic types.

### Vector Type

```c
MPI_Type_vector(count, blocklength, stride, oldtype, &newtype);
```

For strided access (every nth element).

### Indexed Type

```c
MPI_Type_indexed(count, array_of_blocklengths, 
                 array_of_displacements, oldtype, &newtype);
```

For irregular patterns.

---

## Summary

### Key Concepts

1. **Derived Datatypes**
   - Bundle multiple data items
   - Create single message instead of many
   - More efficient communication

2. **Building Derived Types**
   - `MPI_Type_create_struct`: Create from components
   - `MPI_Get_address`: Find memory addresses
   - `MPI_Type_commit`: Prepare for use
   - `MPI_Type_free`: Release resources

3. **Practical Benefits**
   - Reduced message overhead
   - Cleaner code
   - Better scalability
   - Type safety

4. **Barriers**
   - `MPI_Barrier`: Synchronization point
   - All processes wait for all
   - Overhead must be minimized
   - Use only when needed

5. **Performance**
   - Derive types when sending multiple values
   - Avoid unnecessary barriers
   - Minimize synchronization overhead
   - Profile to find bottlenecks

---

## References

- MPI Standard: https://www.mpi-forum.org/
- OpenMPI Documentation: https://www.open-mpi.org/doc/
- Gropp, W., Lusk, E., & Skjellum, A. (1999). Using MPI
- Course Website: https://www.cs.trincoll.edu/~pyoon/cpsc375/
