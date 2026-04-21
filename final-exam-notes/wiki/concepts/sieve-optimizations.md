---
aliases: [Optimized Sieve, Odd-Only Sieve]
tags: [algorithm, optimization, sieve]
sources: [lec24.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Sieve Optimizations

> [!abstract]
> **Sieve Optimizations** refer to techniques used to improve the memory efficiency and execution speed of the Sieve of Eratosthenes. The three most significant optimizations are **deleting even integers**, **eliminating broadcasts**, and **loop reordering**.

## Core Mechanics

### Odd-Only Storage
- Since 2 is the only even prime, only odd numbers are stored in the `marked` array.
- **Benefits**: Cuts memory and computational operations by exactly **50%**.
> [!code] Index Mapping
> ```c
> // Convert index to actual odd number
> #define INDEX_TO_NUMBER(i) (2 * (i) + 3)
> // Convert odd number to local index
> #define NUMBER_TO_INDEX(n) (((n) - 3) / 2)
> ```

### Eliminating Broadcast
- Standard parallel Sieve: Process 0 finds prime $k$ and broadcasts.
- Optimized Sieve: **Every process** runs a sequential Sieve up to $\sqrt{n}$ to find all possible $k$ values. 
- **Redundant Computation Trade-off**:
    - **Pros**: Eliminates network latency of $O(\frac{\sqrt{n}}{\ln \sqrt{n}})$ broadcasts; removes global synchronization for prime generation.
    - **Cons**: Every process performs the exact same work for the first $\sqrt{n}$ numbers.
    - **When it works**: Since $\sqrt{10^8} = 10^4$ is very small, the redundant time is negligible compared to communication delays.

### Loop Reordering (Cache Efficiency)
- **Problem**: Marking multiples like $\{11, 22, 33, \dots\}$ in a huge array causes "random" memory access and frequent **cache misses**.
- **Solution**: Process data in blocks that fit in the cache. Sequential access allows the hardware **prefetcher** to work effectively, providing a 10-100x speedup.

## Implementations & Examples

> [!example] Memory Savings
> For $n = 10^9$, a standard Sieve requires $\approx 1$ GB. An odd-only Sieve requires $500$ MB.

## Connections
* **Prerequisites:** [[sieve-of-eratosthenes]].
* **Used In:** [[cache-optimization-parallel]].
* **Contrasts With:** Naive implementation.
