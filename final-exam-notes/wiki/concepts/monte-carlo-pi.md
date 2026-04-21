---
aliases: [Probabilistic Pi, Pi Estimation]
tags: [algorithm, optimization, openmp]
sources: [Homework 18.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Monte Carlo Pi

> [!abstract]
> **Monte Carlo Pi** is a stochastic algorithm used to estimate the value of $\pi$ by randomly throwing points into a square and counting how many land inside an inscribed circle. It is a standard parallel programming benchmark.

## Core Mechanics

- **Probability**: The ratio of the area of the circle ($r^2\pi$) to the square ($4r^2$) is $\pi/4$.
- **Parallel Strategy**:
    - **Points**: Distribute total throws across processors.
    - **State**: Give each thread a private seed via `firstprivate`.
    - **Aggregation**: Use a `reduction(+:hits)` to sum the results.

## Connections
* **Prerequisites:** [[reduction-operation]], [[data-sharing-clauses]].
* **Used In:** [[Homework-18-Solutions]].
* **Contrasts With:** [[trapezoidal-rule-mpi]] (Deterministic integration).
