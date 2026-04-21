---
tags: [homework, openmp, reduction, monte-carlo, vector-addition]
sources: [Homework 18.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Homework 18

**Source:** Homework 18.pdf
**Date ingested:** 2026-04-20
**Type:** Homework Assignment

## Summary

This assignment provides practical programming exercises in **OpenMP**. It covers parallelizing three common computational patterns: finding a maximum value (reduction), performing vector addition (independent loop iterations), and estimating the value of Pi using a **Monte Carlo method** (requiring both reduction and private state management).

## Key Claims

- Finding the maximum of an array in parallel requires the `reduction(max:var)` clause.
- Local temporary variables inside a parallel for loop are private by default, preventing race conditions.
- Monte Carlo simulations require thread-safe random number generation, typically achieved by giving each thread a private seed via `firstprivate`.

## Entities Mentioned

- [[openmp]] — The parallel API used for implementation.

## Concepts Covered

- [[reduction-operation]] — Using `max` and `+` operators.
- [[parallel-loop-openmp]] — Worksharing iterations.
- [[data-sharing-clauses]] — Using `firstprivate` for seeds.
- [[monte-carlo-pi]] — Stochastic estimation algorithm.
