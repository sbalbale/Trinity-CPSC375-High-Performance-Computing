---
tags: [#source, #exam/theory, #homework/solutions]
sources: [Homework 29.pdf, HW29_Solution.md]
created: 2026-04-21
updated: 2026-04-21
aliases: [HW29 Source]
---

# Homework 29

**Source:** Homework 29.pdf
**Date ingested:** 2026-04-21
**Type:** homework

## Summary

This homework focuses on parallel algorithm optimizations, specifically covering the use of Transpose operations to improve spatial locality in memory, and tracing the execution of Counting Sort including count arrays, prefix sum, and stability placement.

## Key Claims

- Transposing matrices converts inefficient column-wise access into cache-friendly row-wise access, optimizing spatial locality.
- Performing column-wise sorting without transposing on row-mapped distributed systems requires expensive all-to-all communication.
- The prefix sum array in Counting Sort maps values to their final 1-based index positions in the sorted array.
- Iterating from right to left during the placement phase of Counting Sort is required to maintain sorting stability.

## Entities Mentioned

- [[Counting Sort]] — non-comparison based sorting algorithm using count arrays.
- [[Prefix Sum]] — transformation used in counting sort to determine array indices.

## Concepts Covered

- [[Spatial Locality]] — memory access pattern optimization.
- [[Transpose Operation]] — reorganizing data to improve locality.