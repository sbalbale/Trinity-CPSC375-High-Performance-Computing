---
aliases: [LSD Sort, Base Sort]
tags: [algorithm, sorting, mpi, radix]
sources: [lec29.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Radix Sort

> [!abstract]
> **Radix Sort** is a non-comparison sorting algorithm that processes digits of numbers individually, typically starting from the Least Significant Digit (LSD). It relies on a **stable** sorting primitive (like [[counting-sort]]) to maintain the relative order of elements with the same digit.

## Variants

### Least Significant Digit (LSD)
- **Strategy**: Sort from right-to-left (units, then tens, etc.).
- **Requirement**: Must use a **stable** sort subroutine.
- **Pros**: simple to implement, highly parallelizable for fixed-length data.

### Most Significant Digit (MSD)
- **Strategy**: Sort from left-to-right (thousands, then hundreds, etc.).
- **Requirement**: Recursive partitioning.
- **Pros**: Can skip sorting sub-partitions that are already small; better cache locality.

## Practical Considerations

### Base (Radix) Selection
Choosing the base $b$ determines the number of passes $d$ required for a given bit-width (e.g., 32-bit).
| Base ($b$) | Digits ($d$) | Pass Efficiency |
| :--- | :--- | :--- |
| 2 (Binary) | 32 | Very poor (32 passes) |
| 16 (Hex) | 8 | Moderate |
| **256 (Byte)**| **4** | **Optimal for 32-bit** |

## Core Mechanics

### Distributed Parallel Strategy
Parallelizing Radix Sort in a distributed environment (MPI) involves a multi-stage process for each digit pass:
1. **Local Count**: Each process counts the occurrences of digits in its local $n/p$ portion.
2. **Global Coordination**:
    - **`MPI_Exscan`**: Used to compute the **global exclusive prefix sum** of the counts. This determines the starting offset for each process's bucket in the global array.
    - **`MPI_Allreduce`**: Used to find the total count of each bucket across all processes.
3. **Data Redistribution**: Use **`MPI_Alltoallv`** to move elements from their current process to their new owner process based on the calculated global offsets.

> [!warning] Stability Hazard
> In parallel, stability must be maintained **across node boundaries**. This is why `MPI_Exscan` is critical; it ensures that Rank $i$ places its elements after Rank $i-1$ for the same digit value.

## Implementations & Examples

> [!example] Distributed Parallel Radix Sort
> To sort 32-bit unsigned integers with $b=256$ (one byte per digit):
> 1. Each process counts local frequencies for a byte.
> 2. `MPI_Exscan` is used to find the global offsets for each bucket across all processes.
> 3. `MPI_Alltoallv` is used to send the elements to their new destination node.

## Connections
* **Prerequisites:** [[counting-sort]].
* **Used In:** Sorting large datasets of integers or keys (Assignment 8).
* **Contrasts With:** [[compare-and-exchange]] sorting algorithms ($O(n \log n)$).
