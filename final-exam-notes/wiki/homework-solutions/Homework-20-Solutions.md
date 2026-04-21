---
aliases: [Homework 20 Solutions]
tags: [#homework/solutions, #course_hpc, #mpi]
sources: [Homework 20.pdf, lec20.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Homework 20 Solutions

> [!abstract]
> Detailed answers to the conceptual and technical MPI exercises for Homework 20, covering architecture, communication, and synchronization.

## 1. Shared vs. Distributed Memory
| Feature | Shared Memory | Distributed Memory |
| :--- | :--- | :--- |
| **Access** | All processors see a single global address space. | Each processor has its own private memory. |
| **Comm** | Implicit (Reading/Writing shared variables). | Explicit (Sending/Receiving messages). |
| **Advantage** | Easier to program; automatic data locality. | Highly scalable; no bus contention. |
| **Disadvantage** | Limited scalability due to bus traffic. | Complex to program (explicit data movement). |

## 2. MPI Process Characteristics
- **Process Count:** Specified at start-up time (e.g., `mpirun -n 4`). The number of processes remains fixed throughout the execution.
- **Identification:** Each process is identified by a unique integer called a **Rank**, ranging from $0$ to $size-1$.
- **SPMD Model:** **Single Program, Multiple Data**. All processes run the exact same executable, but they execute different branches of logic based on their rank.

## 3. MPI_Init and MPI_Finalize
- **`MPI_Init`**: Initializes the MPI execution environment. It sets up communicators and internal buffers.
- **`MPI_Finalize`**: Cleans up the MPI environment and releases resources.
- **Why encapsulate?** No MPI functions can be called before `Init` or after `Finalize` because the internal state required for communication is not valid.
- **Violation results:** Calling MPI functions outside this window results in **undefined behavior**, typically manifesting as a crash or a "symbol not found" error.

## 4. Communicators
- **Role:** A communicator defines a group of processes that can communicate with each other. It provides a separate communication "universe" to prevent message interference between different modules.
- **`MPI_COMM_WORLD`**: The default communicator that includes **all** processes launched in the job.
- **Usage:**
    - `MPI_Comm_size`: Used to determine the total number of processes available to the program.
    - `MPI_Comm_rank`: Used by each process to determine its own identity so it can decide which subset of data to work on.

## 5. Point-to-Point Mechanics
`MPI_Send` and `MPI_Recv` work together by matching messages.
- **Parameters:**
    - **Destination/Source:** The rank of the target or sender.
    - **Tag:** A user-defined ID to distinguish different types of messages.
    - **Communicator:** The group context (must be the same for sender and receiver).
- **Matching Criteria:** A message is received only if the **communicator**, **sender rank**, and **tag** all match the parameters in the `MPI_Recv` call.

## 6. Dynamic Metadata
- **`MPI_Status`**: A structure that stores metadata about a received message (e.g., source rank and tag). Useful when using `MPI_ANY_SOURCE` or `MPI_ANY_TAG`.
- **`MPI_Get_count`**: Used to determine the actual number of data elements received in a message. This is necessary when the receiver doesn't know the exact message size in advance.

## 7. Communication Issues
- **Blocking Behavior:** `MPI_Recv` is **blocking**, meaning it will not return until the message is fully received. `MPI_Send` is also blocking in the sense that it doesn't return until the buffer is safe to reuse.
- **Deadlock:** If Process 0 calls `MPI_Recv` from Process 1, and Process 1 also calls `MPI_Recv` from Process 0 simultaneously, they will both wait forever. This is a common bug in MPI programs.
- **Buffering:** Smaller messages might be buffered by the system, allowing `MPI_Send` to return even if the matching `Recv` hasn't been called yet. However, relying on this behavior is dangerous because it is implementation-dependent; large messages will cause the sender to block until the receiver starts, potentially leading to deadlock.
