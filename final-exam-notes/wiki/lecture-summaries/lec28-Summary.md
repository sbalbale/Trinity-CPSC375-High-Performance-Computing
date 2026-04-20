---
aliases: [Lecture 28 Summary, Isoefficiency and Sorting II Summary]
tags: [#lecture/detailed, #CPSC375]
sources: [lec28.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Lecture 28: Isoefficiency and Parallel Sorting II

> [!abstract]
> This lecture introduces the **Isoefficiency Function** to quantify scalability and explores **Batcher's parallel sorting algorithms**, including Odd-Even Mergesort and Bitonic Mergesort, which use recursive comparison-exchange patterns to sort in $O(\log^2 n)$ time.

## Isoefficiency and Scalability

### Constant Efficiency
As $p$ increases, efficiency $E$ typically drops because of growing overhead $T_o$. To maintain constant $E$, we must increase the problem size $W$ at a rate that offsets the overhead.

### Derivation
> [!equation]
> $$W = K \cdot T_o$$
> - $K = \frac{E}{1-E}$
> - $T_o = p \cdot T_p - W$ (Total Overhead)
The **Isoefficiency Function** $f(p)$ describes how $W$ must grow relative to $p$.

### Scalability Interpretation
- **$O(p)$**: Highly scalable (only need constant data per processor).
- **$O(p \log p)$**: Scalable (need slight increase in data per processor).
- **$O(p^2)$**: Poorly scalable (need to quadruple data for every doubling of $p$).

---

## Batcher's Sorting Algorithms

### 1. Odd-Even Mergesort
A recursive algorithm to merge two sorted lists $\{a_i\}$ and $\{b_i\}$:
1. **Odd Merge**: Merge odd-indexed elements $a_1, a_3, \dots$ and $b_1, b_3, \dots$ into $\{c_i\}$.
2. **Even Merge**: Merge even-indexed elements $a_2, a_4, \dots$ and $b_2, b_4, \dots$ into $\{d_i\}$.
3. **Refine**: Final list $\{e_i\}$ is obtained by comparing and exchanging adjacent elements from the two merged lists: $e_{2i} = \min(c_{i+1}, d_i), e_{2i+1} = \max(c_{i+1}, d_i)$.

### 2. Bitonic Mergesort
Based on **Bitonic Sequences** (sequences that increase then decrease).
1. **Bitonic Split**: Compare $a_i$ and $a_{i+n/2}$. This produces two smaller bitonic sequences where every element of the first is less than every element of the second.
2. **Recursive Sorting**: Repeatedly split until singleton lists are reached.
3. **Phases**: To sort an arbitrary list, start with pairs, form 4-bit bitonic sequences, then 8-bit, and so on, until a full $n$-bit bitonic sequence is sorted.

> [!equation] Time Complexity
> A bitonic sort for $n = 2^k$ elements requires $\frac{k(k+1)}{2}$ steps, which is **$O(\log^2 n)$** parallel time.
