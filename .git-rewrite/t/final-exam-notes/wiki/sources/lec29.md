---
tags: [mpi, algorithm, sorting, mesh, radix]
sources: [lec29.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Parallel Sorting Algorithms III

**Source:** lec29.txt
**Date ingested:** 2026-04-20
**Type:** Lecture Transcript

## Summary

This lecture explores sorting algorithms tailored for specific network topologies (Mesh) and non-comparison-based sorting algorithms like **Radix Sort**. It details the **Shearsort** algorithm for 2D meshes and the fundamental mechanics of **Counting Sort**, which serves as the stable base for **Radix Sort**. The lecture also outlines the requirements for Assignment 8, a distributed parallel Radix Sort using MPI.

## Key Claims

- **Shearsort** is efficient for meshes because it only requires 1D sorting primitives and converges in $O(\log N)$ iterations for an $N \times N$ mesh.
- **Transposition** can optimize mesh sorting by converting column operations into row operations, improving cache locality and restricting communication to one dimension.
- **Counting Sort** and **Radix Sort** can achieve better than $O(n \log n)$ time by assuming the data is representable as positional digits or bounded integers.
- Parallelizing Radix Sort in a distributed-memory environment requires `MPI_Exscan` and `MPI_Alltoallv` to maintain global stability across nodes.

## Entities Mentioned

- [[mpi]] — Used for implementing parallel Radix Sort primitives.

## Concepts Covered

- [[shearsort]] — 2D mesh sorting algorithm.
- [[counting-sort]] — Stable, frequency-based sorting.
- [[radix-sort]] — Multi-pass positional sorting.
- [[mpi-exscan]] — Exclusive scan used for global offset calculation in parallel Radix Sort.
- [[mpi-alltoallv]] — Variable-sized all-to-all communication used in parallel sorting.
