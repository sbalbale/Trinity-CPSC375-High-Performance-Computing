# Homework 20 — Solution
**CPSC 375: High-Performance Computing | Spring 2026**

---

## Problem 1

**Explain the key differences between shared memory systems and distributed memory systems, including how memory is accessed and how processes communicate. Discuss one advantage and one disadvantage of each model.**

### Solution

**Shared Memory Systems**  
All processors (cores) share a single, unified physical memory space. Any processor can read or write any memory location directly via a load/store instruction. Processes (or threads) communicate implicitly by writing to and reading from shared variables. Examples: multi-core desktop CPUs, SMP servers.

**Distributed Memory Systems**  
Each process has its own **private** memory that no other process can directly access. To exchange data, processes must explicitly send and receive **messages** over an interconnect (e.g., Ethernet, InfiniBand). Examples: HPC clusters, supercomputers, cloud computing nodes. MPI is the standard programming model for distributed memory.

| | Shared Memory | Distributed Memory |
|---|---|---|
| Memory access | Direct — any processor reads/writes any address | Indirect — only the owning process accesses its memory |
| Communication | Implicit through shared variables | Explicit message passing (MPI_Send / MPI_Recv) |
| **Advantage** | Easier to program — no explicit communication; sharing data is free | Scales to thousands of nodes; no bottleneck at a shared memory bus |
| **Disadvantage** | Limited scalability — memory bus becomes a bottleneck; race conditions require synchronization | More complex to program — programmer must manage all data movement explicitly |

---

## Problem 2

**Describe the characteristics of processes in MPI programs, including how many processes exist, how they execute programs, and how they are identified. Explain why MPI programs follow the SPMD model.**

### Solution

**Characteristics of MPI processes:**

- **Fixed count**: The number of processes is specified at launch time (e.g., `mpiexec -n 4 ./program`) and **remains constant** throughout the entire execution of the program. Processes cannot be created or destroyed dynamically.
- **Same program**: Every process runs the **exact same compiled executable** simultaneously from the moment the program starts to the moment it ends. All processes are active throughout execution (unlike OpenMP's fork-join model).
- **Unique rank**: Each process is assigned a unique integer identifier called its **rank**, ranging from `0` to `size - 1`. A process queries its own rank via `MPI_Comm_rank()`.
- **Alternating behavior**: Each process alternately performs local computation and participates in communication operations.

**Why MPI follows SPMD (Single-Program Multiple-Data):**  
Because all processes run the same program, but they operate on **different data** (different subsets of the input). Processes use `if-else` logic on their rank to **differentiate their behavior** — for example, rank 0 might read input and print results while all other ranks do computation. This single-program structure is SPMD: one source file, one binary, multiple concurrent executions with different data and different roles determined at runtime by rank.

---

## Problem 3

**Explain the purpose of `MPI_Init` and `MPI_Finalize`. Why can no MPI calls occur before or after these functions? What could go wrong if this rule is violated?**

### Solution

**`MPI_Init(&argc, &argv)`**  
Initializes the entire MPI execution environment. Internally it:
- Sets up the communication infrastructure (buffers, network connections, process table)
- Assigns each process its rank
- Establishes the default communicator `MPI_COMM_WORLD`

No other MPI function may be called before `MPI_Init` because the data structures, ranks, and communication channels that those functions depend on simply do not exist yet.

**`MPI_Finalize()`**  
Cleans up and tears down everything MPI allocated: frees buffers, closes network connections, releases system resources. After this call the MPI environment is destroyed. No MPI function may be called after `MPI_Finalize` because all MPI state has been released.

**What could go wrong if these rules are violated:**

| Violation | Consequence |
|---|---|
| MPI call before `MPI_Init` | Undefined behavior — the call attempts to use uninitialized data structures, likely causing a **segfault or crash** |
| MPI call after `MPI_Finalize` | Undefined behavior — the communication layer has been torn down; the call may hang, corrupt memory, or crash |
| Skipping `MPI_Finalize` | Resource leaks, incomplete message delivery, or **zombie processes** that hold open network connections |

The rule exists because MPI is a runtime library that requires explicit lifecycle management — it cannot self-initialize like a standard C library.

---

## Problem 4

**Explain the role of communicators in MPI. Define `MPI_COMM_WORLD`. Describe how `MPI_Comm_size` and `MPI_Comm_rank` are used in a parallel program.**

### Solution

**Communicators**  
A communicator is a **named group of processes** that can send messages to each other. All MPI communication operations require a communicator argument — it defines the scope within which messages are exchanged. Communicators enforce message isolation: messages sent within one communicator cannot be received in another, allowing libraries and application code to communicate without interference.

**`MPI_COMM_WORLD`**  
This is the **default communicator** created automatically by `MPI_Init`. It contains **all processes** that were launched with `mpiexec`. Unless the programmer creates custom communicators (for subsets of processes), all communication uses `MPI_COMM_WORLD`.

**`MPI_Comm_size(comm, &size)`**  
Returns the total number of processes in the communicator into the integer pointed to by `&size`. This tells a process how many peers exist, which is essential for dividing work: each process can compute its share as `total_work / size`.

**`MPI_Comm_rank(comm, &rank)`**  
Returns the calling process's unique identifier (rank) into `&rank`. The rank ranges from `0` to `size - 1`. This is how a process knows **who it is** — it uses its rank to determine which portion of the data to process and whether to take a special role (e.g., rank 0 handles I/O).

**Typical usage:**
```c
int size, rank;
MPI_Comm_size(MPI_COMM_WORLD, &size);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);

if (rank == 0) {
    // Only process 0 does this
} else {
    // All other processes do this
}
```

---

## Problem 5

**Explain how `MPI_Send` and `MPI_Recv` work together, including the meaning of parameters such as destination, source, tag, and communicator. Describe how message matching occurs.**

### Solution

**`MPI_Send`** transmits a message from the calling process to a destination process:

```c
int MPI_Send(
    void*        msg_buf_p,    // Pointer to data being sent
    int          msg_size,     // Number of elements to send
    MPI_Datatype msg_type,     // Data type (e.g., MPI_INT, MPI_DOUBLE)
    int          dest,         // Rank of the destination process
    int          tag,          // Message label (programmer-defined integer)
    MPI_Comm     communicator  // Communicator scope
);
```

**`MPI_Recv`** receives a message at the calling process:

```c
int MPI_Recv(
    void*        msg_buf_p,    // Buffer to store incoming data
    int          buf_size,     // Maximum number of elements to receive
    MPI_Datatype buf_type,     // Expected data type
    int          source,       // Rank of the sender (or MPI_ANY_SOURCE)
    int          tag,          // Expected message label (or MPI_ANY_TAG)
    MPI_Comm     communicator, // Communicator scope
    MPI_Status*  status_p      // Output: info about the received message
);
```

**Parameter meanings:**

| Parameter | Role |
|---|---|
| `dest` / `source` | The rank of the target process to send to / receive from |
| `tag` | A programmer-defined integer label that categorizes the message (e.g., `0` for data, `1` for control). Allows distinguishing multiple messages from the same sender |
| `communicator` | Restricts matching to processes within the same communicator group |

**Message matching:**  
A `MPI_Recv` call matches a `MPI_Send` call if and only if **all three** of these match:
1. The **communicator** is the same in both calls
2. The **source rank** in `Recv` matches the **sender's rank** (or `Recv` uses `MPI_ANY_SOURCE`)
3. The **tag** in `Recv` matches the tag in `Send` (or `Recv` uses `MPI_ANY_TAG`)

Messages are matched in the order they arrive; if multiple matching messages exist, the earliest-sent one is received first. This matching system ensures that a process can selectively receive specific types of messages even when multiple messages are in transit simultaneously.

---

## Problem 6

**Explain how a process can receive a message without knowing the sender, message size, or tag in advance. Describe the role of `MPI_Status` and `MPI_Get_count`.**

### Solution

**Receiving without knowing sender/tag:**  
Pass wildcard values in `MPI_Recv`:
- `MPI_ANY_SOURCE` as the `source` argument → accept a message from **any** process
- `MPI_ANY_TAG` as the `tag` argument → accept a message with **any** tag

```c
MPI_Status status;
MPI_Recv(buf, MAX_SIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
         MPI_COMM_WORLD, &status);
```

**`MPI_Status` structure:**  
After `MPI_Recv` returns, the `status` struct is populated with metadata about the message that was actually received:

| Field | Meaning |
|---|---|
| `status.MPI_SOURCE` | The rank of the process that actually sent the message |
| `status.MPI_TAG` | The tag that was attached to the message |
| `status.MPI_ERROR` | Any error code associated with the receive |

This lets the receiving process **discover** who sent the message and what kind of message it was, even if it used wildcards.

**`MPI_Get_count(&status, type, &count)`:**  
Because the receiver may not know the message size in advance (it only knows the maximum buffer size), `MPI_Get_count` queries the `status` to find the **actual number of elements** received:

```c
int count;
MPI_Get_count(&status, MPI_INT, &count);
printf("Received %d ints from process %d with tag %d\n",
       count, status.MPI_SOURCE, status.MPI_TAG);
```

Together, `MPI_Status` and `MPI_Get_count` give a process complete information about an incoming message — sender identity, message type, and exact size — after the fact.

---

## Problem 7

**Discuss the potential issues with `MPI_Send` and `MPI_Recv`, including blocking behavior and implementation-dependent behavior. How can these affect program correctness and performance?**

### Solution

**Issue 1 — `MPI_Recv` always blocks:**  
`MPI_Recv` is a **blocking** call. It does not return until a matching message has been fully received into the buffer. If the expected message never arrives (e.g., the sender crashes or uses the wrong rank/tag), the receiver **hangs indefinitely** — a deadlock.

**Issue 2 — `MPI_Send` behavior is implementation-dependent:**  
The MPI standard does not fully specify when `MPI_Send` returns. Two common behaviors:

| Behavior | Description | When it occurs |
|---|---|---|
| **Buffered (non-blocking)** | MPI copies the message into an internal system buffer and returns immediately before the receiver calls `Recv` | Small messages that fit in the MPI buffer |
| **Synchronous (blocking)** | `MPI_Send` blocks until the receiver has called `MPI_Recv` and begun receiving | Large messages that exceed the buffer size |

This ambiguity means code that works on one MPI implementation or message size may hang on another.

**Issue 3 — Deadlock:**  
The most dangerous consequence. Classic deadlock scenario:

```c
// Process 0              // Process 1
MPI_Send(... to 1 ...);  MPI_Send(... to 0 ...);
MPI_Recv(... from 1 ...) MPI_Recv(... from 0 ...)
```

If both sends are **synchronous** (which happens with large messages), neither process can proceed past its `MPI_Send` because both are waiting for the other's `MPI_Recv` to be called first. Result: **deadlock**.

**Issue 4 — Performance:**  
Even when deadlock does not occur, blocking sends/receives force sequential communication patterns. While one process waits in `MPI_Recv`, its CPU is idle — wasting potential computation time.

**Solutions:**

| Problem | Solution |
|---|---|
| Deadlock from symmetric sends | Alternate roles: one process sends first, the other receives first |
| Blocking performance | Use non-blocking variants: `MPI_Isend` / `MPI_Irecv` (return immediately, use `MPI_Wait` to complete) |
| Implementation ambiguity | Use `MPI_Ssend` (guaranteed synchronous) or `MPI_Bsend` (guaranteed buffered) for predictable behavior |
