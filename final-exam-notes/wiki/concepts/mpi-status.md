---
aliases: [MPI_Status, MPI_Get_count]
tags: [mpi, programming, metadata]
sources: [lec20.txt, lec22.txt]
created: 2026-04-20
updated: 2026-04-20
---

# MPI Status

> [!abstract]
> **MPI_Status** is a structure returned by receiving functions (like `MPI_Recv`) that contains metadata about the received message. It is used to handle situations where the size, sender, or tag of a message is unknown beforehand.

## Core Mechanics

### Structure Fields
The `MPI_Status` structure contains at least three fields:
- `status.MPI_SOURCE`: The rank of the process that sent the message.
- `status.MPI_TAG`: The tag associated with the message.
- `status.MPI_ERROR`: Error code (if any).

### Getting the Message Size
The exact number of elements received is retrieved using a separate function:
> [!code]
> `int MPI_Get_count(MPI_Status* status, MPI_Datatype type, int* count);`

> [!warning] Undefined Counts
> `MPI_Status` does not store the count directly because it depends on the datatype being used to interpret the buffer.

## Implementations & Examples

> [!example] Receiving from Anyone
> ```c
> MPI_Status status;
> MPI_Recv(buffer, 100, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &status);
> printf("Received %d items from Rank %d\n", MPI_Get_count(&status, MPI_INT, &count), status.MPI_SOURCE);
> ```

## Connections
* **Prerequisites:** [[point-to-point-communication]].
* **Used In:** [[master-worker-pattern]] (where the master doesn't know which worker will finish first).
* **Contrasts With:** `MPI_STATUS_IGNORE`.
