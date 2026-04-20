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
- Index $i$ in the local array represents global number $2i+1$.
- **Formula for Start Index**: If $k$ is the current prime, the first multiple to mark is $k^2$. In an odd-only array, the first multiple is $k^2$ if $k^2$ is odd, or $k^2 + k$ if $k^2$ is even (though $k$ being prime means $k^2$ is always odd except for $k=2$).

### Eliminating Broadcast
- Standard parallel Sieve: Process 0 finds prime $k$ and broadcasts.
- Optimized Sieve: **Every process** runs a sequential Sieve up to $\sqrt{n}$ to find all possible $k$ values. 
- **Latency Advantage**: Computational time to find primes up to $\sqrt{n}$ is negligible ($O(\sqrt{n} \log \log \sqrt{n})$) compared to $O(\frac{\sqrt{n}}{\log \sqrt{n}})$ broadcasts across a network.

## Implementations & Examples

> [!example] Memory Savings
> For $n = 10^9$, a standard Sieve requires $\approx 1$ GB. An odd-only Sieve requires $500$ MB.

## Connections
* **Prerequisites:** [[sieve-of-eratosthenes]].
* **Used In:** [[cache-optimization-parallel]].
* **Contrasts With:** Naive implementation.
