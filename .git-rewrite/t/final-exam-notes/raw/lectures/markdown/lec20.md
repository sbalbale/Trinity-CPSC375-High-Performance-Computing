# Lecture 20: Distributed-Memory Programming with MPI

**Course:** CPSC 375 High-Performance Computing  
**Topic:** Message Passing Interface (MPI) Fundamentals

---

## Memory Architecture Models

### Shared-Memory System

```
┌──────────────┐  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐
│  Processor   │  │  Processor   │  │  Processor   │  │  Processor   │
└──────┬───────┘  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘
       │                  │                  │                  │
       └──────────────────┼──────────────────┼──────────────────┘
                          │
                    ┌─────▼─────┐
                    │  Memory   │
                    └───────────┘
```

**Characteristics:**
- All processors access the same memory
- Synchronization through shared variables
- Used in multiprocessor systems (SMPs)
- Examples: OpenMP, pthreads

### Distributed-Memory System

```
┌──────────────┐    ┌──────────────┐    ┌──────────────┐    ┌──────────────┐
│  Processor   │    │  Processor   │    │  Processor   │    │  Processor   │
├─────┬────────┤    ├─────┬────────┤    ├─────┬────────┤    ├─────┬────────┤
│     │        │    │     │        │    │     │        │    │     │        │
│     ▼        │    │     ▼        │    │     ▼        │    │     ▼        │
│   Memory     │    │   Memory     │    │   Memory     │    │   Memory     │
│             │    │             │    │             │    │             │
└─────────────┘    └─────────────┘    └─────────────┘    └─────────────┘
      │                   │                   │                   │
      └───────────────────┼───────────────────┼───────────────────┘
                          │
                    Network (Interconnect)
```

**Characteristics:**
- Each processor has its own memory
- No shared memory (no common address space)
- Communication through explicit message passing
- Processors have unique IDs
- Examples: Clusters, supercomputers, distributed systems

---

## Advantages of Message-Passing Model

### 1. Explicit Memory Management
- Gives the programmer explicit control over memory hierarchy
- Cache locality decisions visible to programmer
- Can optimize data placement and movement
- Understanding memory organization leads to better performance

### 2. Portability
- Message-passing APIs portable to many architectures
- Not dependent on specific hardware memory model
- Works on:
  - Clusters of workstations
  - Supercomputers with custom interconnects
  - Heterogeneous systems
  - Cloud computing platforms

### 3. Deterministic Programming
- Easier to create deterministic (reproducible) programs
- All data transfers are explicit
- No hidden communication through shared memory
- Data flow visible in code

### 4. Simplified Debugging
- Message flow traceable in code
- Communication patterns explicit
- Deadlocks easier to identify
- Performance analysis more straightforward
- Race conditions not an issue (no shared memory)

---

## Processes in Distributed-Memory Systems

### Key Properties

1. **Fixed Number**
   - Number of processes specified at start-up time
   - Remains constant throughout execution of program
   - Cannot create or destroy processes dynamically

2. **Same Program**
   - All processes execute the same program
   - **SPMD Model:** Single Program, Multiple Data
   - Behavior differs based on process rank

3. **Unique Identification**
   - Each process has unique ID number (rank)
   - Ranks numbered 0 to (num_processes - 1)
   - Used for identification in communication

4. **Alternating Computation and Communication**
   - Process alternately performs computations
   - Sends and receives messages
   - Generally blocks while waiting for communication

### Process States

```
Process Lifecycle:
│
├─ Initialization
│  ├─ MPI_Init()
│  └─ Get rank and size
│
├─ Computation Phase
│  ├─ Do local computation
│  └─ Prepare data for communication
│
├─ Communication Phase
│  ├─ Send messages
│  ├─ Receive messages
│  └─ Synchronize if needed
│
├─ Loop back to Computation
│
└─ Finalization
   └─ MPI_Finalize()
```

---

## The Message Passing Interface (MPI)

### Historical Context

**Late 1980s:** Vendor Diversity
- Each computer manufacturer had unique proprietary libraries
- No standardization
- Code not portable across platforms

**1989:** Parallel Virtual Machine (PVM)
- Developed at Oak Ridge National Laboratory
- First attempt at standardized message-passing API
- Provided virtual parallel computer abstraction

**1992:** MPI Standard Development
- Work on Message Passing Interface standard begun
- Major vendors and researchers participated
- Goal: Create portable standard

**1994:** MPI 1.0
- First official version of MPI standard released
- Provided point-to-point and collective communication
- Gained adoption

**1997:** MPI 2.0
- Added dynamic process creation
- Remote memory access (one-sided communication)
- I/O operations

**2012:** MPI 3.0
- Added non-blocking collective operations
- Better support for modern hardware

**2021:** MPI 4.0
- Latest standard
- Continued modernization

### Today

**MPI is the dominant message-passing library standard:**
- Supported by most supercomputer centers
- Wide availability (OpenMPI, MPICH, Intel MPI, etc.)
- Parallel computing standard for distributed systems
- De facto standard for HPC applications

### Language Bindings
- C and C++ bindings
- Fortran bindings (77, 90, 2003+)
- Python bindings (mpi4py)
- Widely used across scientific computing

---

## First MPI Program

### Code Structure

```c
#include <mpi.h>

int main(int argc, char *argv[]) {
    // Initialize MPI
    MPI_Init(NULL, NULL);
    
    // Get communicator size and rank
    int comm_sz;
    int my_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    // Non-master processes send data
    if (my_rank != 0) {
        char greeting[50];
        sprintf(greeting, "Greetings from process %d of %d!", 
                my_rank, comm_sz);
        MPI_Send(greeting, strlen(greeting)+1, MPI_CHAR, 0, 0, 
                 MPI_COMM_WORLD);
    } 
    // Master process receives data
    else {
        printf("Greetings from process %d of %d!\n", my_rank, comm_sz);
        
        char greeting[50];
        for (int q = 1; q < comm_sz; q++) {
            MPI_Recv(greeting, 50, MPI_CHAR, q, 0, 
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("%s\n", greeting);
        }
    }
    
    // Finalize MPI
    MPI_Finalize();
    return 0;
}
```

### Program Logic

**Process 0 (Master):**
1. Print its own greeting
2. Receive greetings from all other processes
3. Print received messages

**Process 1, 2, ..., n-1 (Workers):**
1. Create greeting message with rank information
2. Send greeting to process 0
3. Exit

### Function Explanations

#### MPI_Init
- Initializes MPI
- Must be called before any other MPI routine
- Sets up internal MPI structures
- Parameters usually `NULL, NULL`

#### MPI_Comm_size
```c
MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
```
- Gets total number of processes in communicator
- `comm_sz` = total number of processes
- MPI_COMM_WORLD = default communicator (all processes)

#### MPI_Comm_rank
```c
MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
```
- Gets rank of current process
- `my_rank` = 0 to (comm_sz - 1)
- Used to identify which process is running

#### MPI_Send
```c
MPI_Send(data, count, datatype, dest, tag, comm);
```
- Sends data from current process to destination
- `data`: pointer to data buffer
- `count`: number of elements
- `datatype`: type of elements (e.g., MPI_INT, MPI_CHAR)
- `dest`: rank of destination process
- `tag`: message tag for identification
- `comm`: communicator

#### MPI_Recv
```c
MPI_Recv(buffer, count, datatype, source, tag, comm, status);
```
- Receives data from source process
- `buffer`: pointer to receive buffer
- `count`: maximum number of elements
- `datatype`: type of elements
- `source`: rank of source process
- `tag`: message tag
- `comm`: communicator
- `status`: status object (can be MPI_STATUS_IGNORE)

#### MPI_Finalize
- Cleans up MPI environment
- Must be called after all MPI operations
- Before program termination

---

## Compilation

### Compiler Wrapper Script

MPI provides wrapper scripts to simplify compilation:

```bash
$ mpicc -g -Wall -o mpi_hello mpi_hello.c
```

**Flags:**
- `mpicc`: Wrapper script for C compiler
  - Automatically includes MPI headers
  - Links MPI libraries
  - Sets necessary compiler flags
  
- `-g`: Produces debugging information
  - Needed for gdb and other debuggers
  - Does not affect runtime performance significantly
  
- `-Wall`: Enables all common warnings
  - Helps catch potential bugs
  - Good programming practice
  
- `-o mpi_hello`: Output executable name
  - By default, creates a.out
  - `-o` specifies output file name
  
- `mpi_hello.c`: Input source file

### Equivalent Compilation (without wrapper)
```bash
$ gcc -I/usr/include/mpi -L/usr/lib/mpi -c mpi_hello.c
$ gcc mpi_hello.o -lmpi -o mpi_hello
```

The wrapper script automates this process.

---

## Execution

### MPI Program Execution

```bash
$ mpiexec -n <number_of_processes> <executable>
```

**Example 1: Single Process**
```bash
$ mpiexec -n 1 ./mpi_hello
```

**Output:**
```
Greetings from process 0 of 1 !
```

**Example 2: Four Processes**
```bash
$ mpiexec -n 4 ./mpi_hello
```

**Output:**
```
Greetings from process 0 of 4 !
Greetings from process 1 of 4 !
Greetings from process 2 of 4 !
Greetings from process 3 of 4 !
```

### Execution Notes

1. **Process Order**
   - Processes start and run concurrently
   - Order of process output is non-deterministic
   - May vary between runs

2. **Message Order**
   - Process 0 prints first (in its code)
   - Processes 1, 2, 3 send messages to process 0
   - Process 0 receives and prints them
   - Output order depends on scheduling

3. **Distributed Execution**
   - Each process has independent memory space
   - Process 0 doesn't see process 1's local variables
   - Communication only through explicit MPI calls

---

## MPI Programs: General Structure

### C Program Structure

```c
#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    // Standard includes
    // Must include <mpi.h>
    
    // Initialize MPI
    MPI_Init(&argc, &argv);
    
    // Get rank and size
    int my_rank;
    int comm_sz;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    
    // Do work
    // ... computation and communication ...
    
    // Finalize MPI
    MPI_Finalize();
    
    return 0;
}
```

### Standard Includes
- `<stdio.h>`: Standard I/O (printf, etc.)
- `<stdlib.h>`: Memory allocation (malloc, etc.)
- `<string.h>`: String operations (strlen, sprintf, etc.)
- `<mpi.h>`: MPI declarations and definitions
- **Must include `<mpi.h>`** for any MPI program

### MPI Naming Conventions

All MPI identifiers start with "MPI_":
- **Function names:** `MPI_Send`, `MPI_Recv`, `MPI_Init`
- **Data types:** `MPI_INT`, `MPI_DOUBLE`, `MPI_CHAR`
- **Constants:** `MPI_COMM_WORLD`, `MPI_STATUS_IGNORE`

**Naming Pattern:**
- `MPI_` prefix for all MPI identifiers
- First letter after underscore is uppercase
- Helps distinguish MPI from application code
- Avoids naming conflicts

---

## MPI Components Overview

### Initialization
```c
int MPI_Init(int *argc_p, char ***argv_p);
```
- Must be called first in MPI program
- Initializes MPI runtime system
- Sets up internal data structures
- Returns error code (MPI_SUCCESS = 0)

### Communicator Operations
```c
int MPI_Comm_size(MPI_Comm comm, int *size);
int MPI_Comm_rank(MPI_Comm comm, int *rank);
```
- Query communicator information
- Essential for SPMD programs

### Point-to-Point Communication
```c
int MPI_Send(void *buf, int count, MPI_Datatype dtype,
             int dest, int tag, MPI_Comm comm);

int MPI_Recv(void *buf, int count, MPI_Datatype dtype,
             int source, int tag, MPI_Comm comm,
             MPI_Status *status);
```
- Basic send and receive operations
- Blocking operations (wait for completion)

### Collective Communication
- `MPI_Bcast`: Broadcast from one process to all
- `MPI_Reduce`: Reduce (combine) data from all processes
- `MPI_Allreduce`: Reduce with result to all processes
- `MPI_Scatter`: Distribute data from one process
- `MPI_Gather`: Collect data to one process

### Finalization
```c
int MPI_Finalize(void);
```
- Must be called last
- Cleans up MPI resources
- No MPI calls after MPI_Finalize

---

## Summary

### Key Concepts

1. **Distributed Memory**
   - Each process has independent memory
   - Explicit message passing for communication
   - Portable and deterministic

2. **MPI Standard**
   - Dominant message-passing API
   - Widely supported and standardized
   - Scalable to thousands of processes

3. **Process Model**
   - Fixed number of processes
   - All run the same SPMD program
   - Identified by rank

4. **Basic MPI Program**
   - Initialize with MPI_Init
   - Get rank and size
   - Communicate with MPI_Send/MPI_Recv
   - Finalize with MPI_Finalize

5. **Compilation and Execution**
   - Use mpicc wrapper for compilation
   - Use mpiexec for execution
   - Specify number of processes with -n

### Next Topics
- Point-to-point communication patterns
- Collective communication operations
- Derived datatypes
- Synchronization and barriers

---

## References

- MPI Standard: https://www.mpi-forum.org/
- Open MPI: https://www.open-mpi.org/
- MPICH: https://www.mpich.org/
- Gropp, W., Lusk, E., & Skjellum, A. (1999). Using MPI
- Pacheco, P. S. (2011). An Introduction to Parallel Programming
- Course Website: https://www.cs.trincoll.edu/~pyoon/cpsc375/
