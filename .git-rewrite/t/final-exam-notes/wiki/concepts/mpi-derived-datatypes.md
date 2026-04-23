---
aliases: [Custom Datatypes, MPI_Type_create_struct]
tags: [mpi, programming, performance]
sources: [lec22.txt]
created: 2026-04-20
updated: 2026-04-20
---

# MPI Derived Datatypes

> [!abstract]
> **MPI Derived Datatypes** allow representing collections of heterogeneous data items in memory by storing their types and relative displacements. Their primary purpose is to **reduce communication overhead** by bundling multiple data items into a single `MPI_Send`/`MPI_Recv` operation.

## Motivation: The Multi-Message Problem
Sending multiple related values (e.g., `double a`, `double b`, `int n`) using individual `MPI_Send` calls is inefficient.
- **Overhead**: Each message has a header and routing information; 3 messages have 3x the overhead.
- **Congestion**: Multiple messages increase network traffic and latency, especially at scale.
**Solution**: Derived datatypes bundle these items into a single memory representation for one communication operation.

## Advanced Datatypes
Beyond structures, MPI provides specialized constructors for common memory patterns:
- **`MPI_Type_contiguous`**: Groups a fixed number of elements of the same type (e.g., a simple array).
- **`MPI_Type_vector`**: For strided access, such as taking every $n$-th element or a column from a row-major matrix.
- **`MPI_Type_indexed`**: For irregular data patterns where displacements and block lengths vary.

## Core Mechanics

### Memory Addressing
- **`MPI_Aint`**: A special integer type large enough to store a memory address. It ensures **portability** across different 32-bit and 64-bit architectures.
- **`MPI_Get_address`**: Mandatory for calculating displacements. Compilers often insert **padding bytes** for hardware alignment (e.g., between a `char` and a `double`), making manual byte counting unreliable.

> [!code] Key Functions
> ```c
> // Builds a struct-like datatype
> int MPI_Type_create_struct(int count, int blocklengths[], MPI_Aint displacements[], MPI_Datatype types[], MPI_Datatype* new_type);
> 
> // Gets memory address for displacement calculation
> int MPI_Get_address(void* location, MPI_Aint* address);
> 
> // Must be called before use
> int MPI_Type_commit(MPI_Datatype* datatype);
> 
> // Frees storage after use
> int MPI_Type_free(MPI_Datatype* datatype);
> ```

> [!warning] Common Pitfalls
> - **Commitment:** Forgetting to call `MPI_Type_commit` before using the type in a communication function will result in an error.
> - **Memory Leaks:** Always call `MPI_Type_free` when the datatype is no longer needed.
> - **Displacement Calculation:** Displacements should be calculated relative to the first element's address using `MPI_Get_address` to ensure portability.

## Implementations & Examples

> [!example] Bundling Input Parameters
> Instead of sending `double a`, `double b`, and `int n` separately, a single derived type can be created:
> 1. `array_of_blocklengths = {1, 1, 1}`
> 2. `array_of_types = {MPI_DOUBLE, MPI_DOUBLE, MPI_INT}`
> 3. `displacements[1] = &b - &a`, `displacements[2] = &n - &a` (using `MPI_Get_address`).

## Connections
* **Prerequisites:** [[mpi]], basic data types.
* **Used In:** Complex scientific simulations where heterogeneous data needs to be shared.
* **Contrasts With:** [[collective-communication]] (which often uses basic types but could use derived types).
