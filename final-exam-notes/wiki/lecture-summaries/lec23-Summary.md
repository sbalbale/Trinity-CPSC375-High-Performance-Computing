---
aliases: [Lecture 23 Summary, Parallel Sieve Summary]
tags: [#lecture/detailed, #CPSC375]
sources: [lec23.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Lecture 23: Parallel Sieve of Eratosthenes

> [!abstract]
> This lecture explores the parallelization of the **Sieve of Eratosthenes** using **Block Decomposition**. It focuses on the mathematical foundations for mapping data to processes, implementing the parallel loop, and analyzing the resulting performance.

## Data Decomposition Strategies
To parallelize the sieve, the range of numbers $[2, n]$ must be divided among $p$ processes.

### Interleaved (Cyclic) Decomposition
- **Definition**: Elements are assigned to processes in a round-robin fashion (e.g., process $i$ gets indices $i, i+p, i+2p, \dots$).
- **Drawback**: Causes **load imbalance**. In the Sieve, smaller numbers have more multiples to mark, and the density of primes decreases for larger numbers. Processes with smaller indices will do significantly more work.

### Block Decomposition
- **Definition**: The range is divided into $p$ contiguous blocks of size $\approx n/p$.
- **Benefits**: Better load balancing for the Sieve algorithm.

#### Block Mapping Macros (Method #2)
> [!code] Standard Macros
> ```c
> #define BLOCK_LOW(id,p,n)  ((id)*(n)/(p))
> #define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1)
> #define BLOCK_SIZE(id,p,n) (BLOCK_LOW((id)+1,p,n) - BLOCK_LOW(id,p,n))
> #define BLOCK_OWNER(index,p,n) (((p)*(index)+1)-1)/(n))
> ```

## Parallel Implementation
The algorithm iterates through primes $k$ where $k^2 \le n$.

1. **Initialization**: Each process allocates its local block of memory.
2. **Sieving Loop**:
    - **Identify Prime**: Process 0 identifies the next prime $k$.
    - **Broadcast**: Process 0 uses `MPI_Bcast` to send $k$ to all other processes.
    - **Local Marking**: Each process marks multiples of $k$ within its **local block**.
3. **Termination**: Loop ends when $k^2 > n$.
4. **Result Gathering**: Use `MPI_Reduce` to count the total number of unmarked numbers (primes).

> [!warning] Key Assumption
> The parallel algorithm assumes $p < \sqrt{n}$. This ensures that **Process 0** always contains all the primes $k$ necessary to sieve the entire range, as the largest possible $k$ is $\sqrt{n}$.

## Performance Analysis
The expected execution time $T_p$ is modeled as:

> [!equation] Parallel Sieve Complexity
> $$T_p \approx \frac{\chi n \ln \ln n}{p} + \left( \frac{\sqrt{n}}{\ln \sqrt{n}} \right) \lambda \lceil \log p \rceil$$
> - $\chi$: Time to mark a single cell.
> - $\lambda$: Communication latency.
> - First term: Parallelized computation.
> - Second term: Communication overhead of $O(\sqrt{n}/\ln \sqrt{n})$ broadcasts.
