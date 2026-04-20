---
aliases: [Sieve]
tags: [#exam/algorithm, #number-theory]
sources: [lec16.pdf]
created: 2025-05-15
updated: 2025-05-15
---

# Sieve of Eratosthenes

> [!abstract] TL;DR Summary
> An efficient algorithm for finding all prime numbers up to a specified limit $n$. In parallel, it requires **data decomposition** and **broadcasting** of prime factors.

## Core Mechanics

> [!equation] The Prime Rule
> To find primes up to $n$, you only need to sieve using primes up to $\sqrt{n}$.

### Parallel Implementation
1. **Decomposition:** Divide the range $[2, n]$ among processors.
2. **Broadcast:** Each processor needs to know the "current" prime to mark its multiples.
3. **Synchronization:** Processors must wait for the next prime to be identified before proceeding.

> [!warning] Common Pitfalls
> * **Communication Frequency:** Broadcasting every single prime can be expensive. Optimization involves sieving in blocks.
> * **Load Imbalance:** Higher ranges have fewer primes/multiples to mark; however, for the Sieve, work is usually proportional to block size.

## Connections
* **Example of:** [[Domain Decomposition]]
* **Requires:** [[Communication]] (Broadcast)
