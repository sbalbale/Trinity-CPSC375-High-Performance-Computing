---
aliases: [Homework 24 Solutions]
tags: [#homework/solutions, #course_hpc, #mpi, #sieve]
sources: [Homework 24.pdf, lec24.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Homework 24 Solutions

> [!abstract]
> Solutions to the Sieve of Eratosthenes optimization exercises for Homework 24, featuring code for odd-only memory management and broadcast-free synchronization.

## Optimization 1: Odd-Only Storage (Deleting Evens)

**Concept:** 2 is the only even prime. We can treat 2 as a special case and store only odd numbers in the local `marked` array.

### Mathematical Mapping
> [!equation] Indexing Formulas
> For a local array index $i$, the corresponding global number is:
> $$\text{Number}(i) = 2 \times (\text{Global\_Index}(i)) + 3$$
> To find the first multiple of prime $k$ in a local block:
> 1. Find the first global multiple $M = k^2$ (or greater if $k^2$ is below the local low).
> 2. If $M$ is even, $M = M + k$.
> 3. Local Index $i = (M - \text{Local\_Low\_Value}) / 2$.

> [!code] Implementation Snippet
> ```c
> // Size is halved
> long size = (BLOCK_SIZE(id, p, n) + 1) / 2;
> char* marked = (char*)malloc(size);
> 
> // Loop increments by 2k
> for (long j = first_multiple_index; j < size; j += k) {
>     marked[j] = 1;
> }
> ```

---

## Optimization 2: Eliminating Broadcast

**Concept:** Instead of Process 0 finding the next prime and calling `MPI_Bcast`, **every process** performs a sequential sieve on a small private array to find all primes up to $\sqrt{n}$.

### Logic Flow
1. Each process allocates a small array `pre_sieve` of size $\sqrt{n}$.
2. Each process performs a sequential sieve on `pre_sieve`.
3. Each process then iterates through `pre_sieve` to find sieving primes and applies them to its large local `marked` block.

> [!code] Modified Parallel Loop
> ```c
> // 1. Local Sequential Sieve up to sqrt(n)
> sequential_sieve(sqrt_n, pre_sieve_array);
> 
> // 2. No MPI_Bcast needed! 
> // Each process already has all primes up to sqrt(n)
> for (long k = 3; k <= sqrt_n; k += 2) {
>     if (!pre_sieve_array[k]) {
>         // Apply prime k to local block
>         mark_local_multiples(k, local_marked_array);
>     }
> }
> ```

---

## Performance Comparison Summary

| Version | Memory Usage | Comm. Overhead | Computation |
| :--- | :--- | :--- | :--- |
| **Original** | $n$ bytes | $O(\frac{\sqrt{n}}{\ln \sqrt{n}} \log p)$ | $O(\frac{n \ln \ln n}{p})$ |
| **Odd-Only** | $n/2$ bytes | $O(\frac{\sqrt{n}}{\ln \sqrt{n}} \log p)$ | $O(\frac{n \ln \ln n}{2p})$ |
| **Broadcast-Free** | $n/2$ bytes | **None** (for primes) | $O(\frac{n \ln \ln n}{2p} + \sqrt{n} \log \log \sqrt{n})$ |

**Benchmark Conclusion:**
The **Broadcast-Free Odd-Only Sieve** is significantly faster on large clusters. While the sequential sieve adds $O(\sqrt{n})$ work to every process, this is negligible compared to the network latency of hundreds of broadcasts, especially as the number of processes ($p$) increases.
