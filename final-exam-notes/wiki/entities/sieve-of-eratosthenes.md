---
aliases: [Sieve, performance-analysis-sieve]
tags: [#exam/algorithm, #number-theory]
sources: [lec16.pdf, lec23.txt]
created: 2025-05-15
updated: 2026-04-20
---

# Sieve of Eratosthenes

> [!abstract] TL;DR Summary
> An efficient algorithm for finding all prime numbers up to a specified limit $n$. In parallel, it requires **block decomposition** and **broadcasting** of prime factors identified by a root process.

## Core Mechanics

### Why Parallelize?
For very large $n$ (e.g., $10^9$), the sequential algorithm becomes impractical:
- **Memory**: Requires an array of size $n$ (~1 GB for $10^9$).
- **Time**: $O(n \ln \ln n)$ operations ($>10^9$) take several seconds.

> [!equation] The Prime Rule
> To find primes up to $n$, you only need to sieve using primes up to $\sqrt{n}$.

### Parallel Implementation
1. **Decomposition:** Divide the range $[2, n]$ using [[block-decomposition]]. Interleaved (cyclic) distribution is avoided due to significant **load imbalance**.
2. **Local Initialization:** Each process allocates a local array for its block, mapping [[local-vs-global-indices]].
3. **Broadcast:** Process 0 identifies the next prime $k$ and uses `MPI_Bcast` to send it to all processes.
4. **Marking:** Each process marks multiples of $k$ in its local range.
5. **Reduction:** After sieving up to $\sqrt{n}$, use `MPI_Reduce` with `MPI_SUM` to count primes.

## Data Decomposition Comparison

### Interleaved (Cyclic) Decomposition
- **Assignment**: Process $i$ gets elements $i, i+p, i+2p, \dots$
- **Problem**: **Severe Load Imbalance**.
    - Early primes (like 2) leave very few unmarked elements for processes assigned to them.
    - Example ($p=4, n=31$): Process 0 handles $\{2, 6, 10, \dots\}$. All are composite (except 2), so P0 has almost zero work later. Processes assigned odd numbers do significantly more marking work.

### Block Decomposition
- **Assignment**: Process $i$ gets a contiguous chunk of size $\approx n/p$.
- **Benefits**:
    - **Better Locality**: Contiguous access improves cache performance.
    - **Balanced Workload**: Initial work is equal and distribution remains stable.
    - **Simple Communication**: Fewer messages needed between neighbors.

## Detailed Implementation

### Finding the First Multiple
One of the most critical steps in the parallel implementation is determining the starting index for marking in a process's local block.

> [!code] First Multiple Calculation
> ```c
> // Determine where to start marking in local array
> if (prime * prime > low_value)
>     first = prime * prime - low_value;  // Start from prime²
> else {
>     // Calculate first multiple of prime >= low_value
>     if (!(low_value % prime))
>         first = 0;  // low_value is divisible by prime
>     else
>         first = prime - (low_value % prime);
> }
> ```

### Main Sieving Loop
The loop continues until the square of the current prime exceeds the global limit $n$.
1. **Mark**: Each process marks multiples of the current `prime` starting from `first`.
2. **Find Next**: Process 0 identifies the next unmarked index in its local block.
3. **Broadcast**: Process 0 broadcasts the new `prime` value to all other processes.

## Performance & Complexity

> [!equation] Parallel Time Complexity
> $$T_p \approx \frac{\chi n \ln \ln n}{p} + \left( \frac{\sqrt{n}}{\ln \sqrt{n}} \right) \lambda \lceil \log p \rceil$$
> - $\chi$: Marking time per element.
> - $\lambda$: Latency of a single broadcast.
> - The $O(\sqrt{n}/\ln \sqrt{n})$ term represents the number of broadcasts required (number of primes up to $\sqrt{n}$).

## Connections
* **Example of:** [[domain-decomposition]]
* **Requires:** [[collective-communication]] (`MPI_Bcast`, `MPI_Reduce`)
* **Uses:** [[block-decomposition]], [[sieve-optimizations]]

