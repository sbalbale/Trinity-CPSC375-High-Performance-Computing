---
aliases: [Lecture 22 Summary]
tags: [#lecture/detailed, #CPSC375]
sources: [lec22.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Lecture 22: Derived Types and Barriers

> [!abstract]
> This lecture focuses on **MPI Derived Datatypes** for efficient communication of heterogeneous data and **MPI Barriers** for process synchronization. It also provides a high-level overview of Assignment 6's concurrent transaction system and **ACID properties**.

## Heterogeneous Data Communication
Initially, communicating multiple variables (like two `doubles` and an `int`) required multiple `MPI_Send` and `MPI_Recv` calls. This is inefficient due to the overhead of multiple messages.

### Derived Datatypes
**Derived datatypes** allow MPI to represent any collection of data items in memory by storing their types and relative locations (displacements).

> [!code] MPI Derived Type Functions
> - `MPI_Type_create_struct`: Builds a derived datatype with different basic types.
> - `MPI_Get_address`: Finds the memory address of a variable (stored in `MPI_Aint`).
> - `MPI_Type_commit`: Optimizes the new type for communication.
> - `MPI_Type_free`: Frees the storage used by the derived type.

The process involves:
1. Defining block lengths (e.g., `{1, 1, 1}`).
2. Defining the types (e.g., `{MPI_DOUBLE, MPI_DOUBLE, MPI_INT}`).
3. Calculating displacements using `MPI_Get_address`.
4. Creating and committing the type.

## Synchronization and Timing
### MPI_Barrier
> [!equation]
> `int MPI_Barrier(MPI_Comm comm);`

Ensures that no process returns from the call until **every** process in the communicator has started calling it. This is crucial for synchronizing processes before starting a timed operation.

### Performance Measurement
`MPI_Wtime()` returns the number of seconds elapsed since a fixed point in the past. To time a parallel code block:
1. Call `MPI_Barrier(comm)`.
2. Record `local_start = MPI_Wtime()`.
3. Execute the code.
4. Record `local_finish = MPI_Wtime()`.
5. Use `MPI_Reduce` with `MPI_MAX` to find the total elapsed time across all processes.

## Transaction Processing (Assignment 6)
The lecture reviews the **ACID Properties** for the concurrent transaction processing system:
- **Atomicity**: Operations are an all-or-nothing unit.
- **Consistency**: Transition between valid states only.
- **Isolation**: Concurrent execution results match sequential execution.
- **Durability**: Committed changes persist after crashes.

The system architecture involves a **Master/Worker pattern** using a shared work queue and a database for persistence.
