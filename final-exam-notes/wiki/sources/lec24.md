---
tags: [mpi, optimization, sieve, cache, dsm]
sources: [lec24.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Sieve Implementation and Cache Coherence

**Source:** lec24.txt
**Date ingested:** 2026-04-20
**Type:** Lecture Transcript

## Summary

This lecture concludes the **Sieve of Eratosthenes** implementation in MPI, focusing on specific code structures and advanced optimizations. It also introduces concepts related to **Distributed Shared-Memory (DSM)** systems, specifically **cache coherence protocols** like Write Through and Write Once.

## Key Claims

- **Deleting even integers** from the Sieve doubles the potential range $n$ for a given memory size and halves computation.
- **Independent computation** of sieving primes up to $\sqrt{n}$ by each process can be faster than broadcasting them, as it replaces communication with redundant but fast local calculation.
- **Loop reordering** is a powerful technique to improve the **cache hit rate** in parallel algorithms.
- **Write Once protocol** reduces bus traffic by allowing a process to keep data in a **DIRTY** state, meaning it has the only valid copy and memory is not yet updated.

## Entities Mentioned

- [[sieve-of-eratosthenes]] — Implementation and optimization details.
- [[mpi]] — Used for the base parallel implementation.

## Concepts Covered

- [[sieve-optimizations]] — Deleting evens, eliminating broadcast, and loop reordering.
- [[cache-optimization-parallel]] — Improving hit rates in data-intensive parallel loops.
- [[cache-coherence-protocols]] — Write Through and Write Once (MESI-like) protocols.
- [[distributed-shared-memory]] — Systems where memory is physically distributed but logically shared.
