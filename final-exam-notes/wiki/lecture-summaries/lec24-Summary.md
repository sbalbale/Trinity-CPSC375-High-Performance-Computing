---
aliases: [Lecture 24 Summary, Sieve Optimization Summary]
tags: [#lecture/detailed, #CPSC375]
sources: [lec24.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Lecture 24: Sieve Implementation and Cache Coherence

> [!abstract]
> This lecture covers the final implementation details of the parallel Sieve algorithm and introduces three major optimizations: odd-only storage, broadcast elimination, and cache-friendly loop reordering. It also transitions into **Distributed Shared-Memory (DSM)** systems and **cache coherence protocols**.

## Sieve Optimizations

### 1. Odd-Only Storage (Deleting Evens)
- **Concept**: Since 2 is the only even prime, we can store only odd numbers in the `marked` array.
- **Benefit**:
    - Reduces memory usage by **50%**, allowing larger $n$.
    - Reduces computational steps by **50%**.

### 2. Eliminating Broadcast
- **Problem**: Broadcasting every prime $k$ up to $\sqrt{n}$ from Process 0 introduces communication overhead.
- **Solution**: Every process computes all primes up to $\sqrt{n}$ independently. 
- **Benefit**: Redundant computation is much faster than network latency, especially for large $p$.

### 3. Loop Reordering (Cache Optimization)
- **Problem**: The standard algorithm marks multiples of $k$ across a large array, leading to frequent **cache misses** as it "jumps" through memory.
- **Solution**: Divide the local block into sub-blocks that fit in the L1/L2 cache. For each sub-block, apply all sieving primes before moving to the next sub-block.
- **Benefit**: Drastically improves the **cache hit rate**.

---

## Distributed Shared-Memory (DSM)
Systems that provide a **global address space** across physically distributed memory modules. This requires protocols to maintain **cache coherence**.

### Write Through Protocol
- On a **Write Hit**: Update both the local cache and main memory immediately.
- On a **Read Miss**: Fetch from memory; other caches are not affected.
- **Pros**: Simple to implement.
- **Cons**: High bus traffic due to constant memory updates.

### Write Once Protocol
Uses three states for cache blocks:
1. **INVALID**: Block data is not valid.
2. **CLEAN**: Cache matches memory; other caches may have it.
3. **DIRTY**: Cache is the only valid copy; memory is stale.

> [!example] Write Once Workflow
> - First write to a CLEAN block: Update memory, invalidate others, mark as **CLEAN**.
> - Subsequent writes to a CLEAN block: Mark as **DIRTY**, do NOT update memory (reduces traffic).
> - If another CPU reads a DIRTY block: The owner provides the data, memory is updated, and both set to **CLEAN**.
