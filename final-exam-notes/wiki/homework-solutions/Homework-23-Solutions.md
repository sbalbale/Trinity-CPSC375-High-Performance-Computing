---
aliases: [Homework 23 Solutions]
tags: [#homework/solutions, #course_hpc]
sources: [Homework 23.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Homework 23 Solutions

> [!abstract]
> Solutions to the data decomposition exercises for Homework 23, including analysis of idle process conditions and a detailed comparison of block mapping methods.

## Problem 1: Naive Block Allocation

**Scenario:** $n$ items, $p$ processes. First $p-1$ processes get $\lceil n/p \rceil$ items. Last process gets the remainder.

### A. Last Process Gets Zero Elements
> [!question]
> Find values for $n$ and $p$ where the last process does not get any elements.

**Condition:** $(p-1) \lceil n/p \rceil \ge n$
**Solution:** 
Let $n=3, p=4$.
- $\lceil 3/4 \rceil = 1$.
- Processes 0, 1, and 2 each receive 1 element.
- Total assigned = 3.
- **Process 3 receives 0 elements.**

### B. $\lfloor p/2 \rfloor$ Processes Get Zero Elements
> [!question]
> Find values for $n$ and $p$ where $\lfloor p/2 \rfloor$ processes do not get any values ($p > 1$).

**Solution:**
Let $p=4$. We need $\lfloor 4/2 \rfloor = 2$ processes to get zero. This means the first $4-2=2$ processes must account for all $n$ elements.
- Let $n=2$.
- $\lceil 2/4 \rceil = 1$.
- Processes 0 and 1 each receive 1 element.
- Total assigned = 2.
- **Processes 2 and 3 receive 0 elements.**

---

## Problem 2: Comparison of Block Decomposition Strategies

We compare two schemes for assigning $n$ elements to $p$ processes:
- **Scheme 1 (Remainder Distribution):** Calculate $r = n \pmod p$. The first $r$ processes get $\lceil n/p \rceil$, and the remaining get $\lfloor n/p \rfloor$.
- **Scheme 2 (Formula Mapping):** Low index $L(id) = \lfloor (id \cdot n) / p \rfloor$. High index $H(id) = L(id+1) - 1$.

### Detailed Assignment Table

| Scenario | Scheme 1 (Sizes) | Scheme 2 (Sizes) | Scheme 2 Indices (Low, High) |
| :--- | :--- | :--- | :--- |
| **A. n=15, p=4** | 4, 4, 4, 3 | 3, 4, 4, 4 | P0:[0,2], P1:[3,6], P2:[7,10], P3:[11,14] |
| **B. n=15, p=6** | 3, 3, 3, 2, 2, 2 | 2, 3, 2, 3, 2, 3 | P0:[0,1], P1:[2,4], P2:[5,6], P3:[7,9], P4:[10,11], P5:[12,14] |
| **C. n=16, p=5** | 4, 3, 3, 3, 3 | 3, 3, 4, 3, 3 | P0:[0,2], P1:[3,5], P2:[6,9], P3:[10,12], P4:[13,15] |
| **D. n=18, p=4** | 5, 5, 4, 4 | 4, 5, 4, 5 | P0:[0,3], P1:[4,8], P2:[9,12], P3:[13,17] |
| **E. n=20, p=6** | 4, 4, 3, 3, 3, 3 | 3, 3, 4, 3, 3, 4 | P0:[0,2], P1:[3,5], P2:[6,9], P3:[10,12], P4:[13,15], P5:[16,19] |
| **F. n=23, p=7** | 4, 4, 3, 3, 3, 3, 3| 3, 3, 4, 3, 3, 4, 3| P0:[0,2], P1:[3,5], P2:[6,9], P3:[10,12], P4:[13,15], P5:[16,19], P6:[20,22] |

### Observations
1. **Balance:** Both schemes ensure that block sizes differ by at most 1, achieving excellent **load balancing**.
2. **Remainder Placement:** Scheme 1 consistently places larger blocks at the beginning of the range. Scheme 2 distributes the "extra" elements more evenly across the set of processes.
3. **Complexity:** Scheme 2 is often easier to implement in SPMD code because a process can determine its range using its own `id` without knowing whether it is part of the "remainder" group.
