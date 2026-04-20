---
aliases: [Sieve]
tags: [#exam/algorithm, #number-theory]
sources: [lec16.pdf, lec23.txt]
created: 2025-05-15
updated: 2026-04-20
---

# Sieve of Eratosthenes

> [!abstract] TL;DR Summary
> An efficient algorithm for finding all prime numbers up to a specified limit $n$. In parallel, it requires **block decomposition** and **broadcasting** of prime factors identified by a root process.

## Core Mechanics

> [!equation] The Prime Rule
> To find primes up to $n$, you only need to sieve using primes up to $\sqrt{n}$.

### Parallel Implementation
1. **Decomposition:** Divide the range $[2, n]$ using [[block-decomposition]]. Interleaved (cyclic) distribution is avoided due to significant **load imbalance**.
2. **Local Initialization:** Each process allocates a local array for its block, mapping [[local-vs-global-indices]].
3. **Broadcast:** Process 0 identifies the next prime $k$ and uses `MPI_Bcast` to send it to all processes.
4. **Marking:** Each process marks multiples of $k$ in its local range.
5. **Reduction:** After sieving up to $\sqrt{n}$, use `MPI_Reduce` with `MPI_SUM` to count primes.

> [!warning] Load Imbalance in Cyclic Sieve
> In an interleaved distribution, processes with smaller indices handle smaller numbers. These numbers have a higher density of multiples to mark, causing those processes to work much longer than those handling larger numbers.

## Performance & Complexity

> [!equation] Parallel Time Complexity
> $$T_p \approx \frac{\chi n \ln \ln n}{p} + \left( \frac{\sqrt{n}}{\ln \sqrt{n}} \right) \lambda \lceil \log p \rceil$$
> - $\chi$: Marking time per element.
> - $\lambda$: Latency of a single broadcast.
> - The $O(\sqrt{n}/\ln \sqrt{n})$ term represents the number of broadcasts required (number of primes up to $\sqrt{n}$).

## Connections
* **Example of:** [[domain-decomposition]]
* **Requires:** [[collective-communication]] (`MPI_Bcast`, `MPI_Reduce`)
* **Uses:** [[block-decomposition]]

