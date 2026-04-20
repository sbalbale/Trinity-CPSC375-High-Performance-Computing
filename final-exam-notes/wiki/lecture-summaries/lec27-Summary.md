---
aliases: [Lecture 27 Summary, Performance Analysis Summary]
tags: [#lecture/detailed, #CPSC375]
sources: [lec27.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Lecture 27: Performance Analysis

> [!abstract]
> This lecture covers the critical metrics and laws governing parallel performance. It focuses on the mathematical foundations for measuring speedup, efficiency, and scalability, and provides tools for diagnosing why parallel programs fail to achieve linear speedup.

## Core Metrics

### Speedup and Efficiency
- **Speedup ($S_p$)**: The ratio of the serial execution time to the parallel execution time on $p$ processors. $S_p = T_s / T_p$.
- **Efficiency ($E$)**: The speedup per processor. $E = S_p / p$.

## Two Views of Scaling

### Amdahl's Law (Fixed Workload)
> [!equation]
> $$S_p = \frac{1}{f + \frac{1-f}{p}}$$
> - $f$: Serial fraction of the program.
> - $(1-f)$: Parallelizable fraction.
> - $p$: Number of processors.
> As $p \to \infty$, $S_p \to 1/f$. Even with infinite cores, a 10% serial fraction limits speedup to 10.

### Gustafson's Law (Scaled Workload)
> [!equation]
> $$S_p = p + (1-p)f$$
> Focuses on **scaled speedup**. As we add more processors, we also increase the problem size, allowing the parallel portion to grow relative to the serial portion.

---

## Diagnosing Performance Bottlenecks

### The Karp-Flatt Metric ($e$)
Used to experimentally determine why an algorithm doesn't scale well.
> [!equation]
> $$e = \frac{1/S_p - 1/p}{1 - 1/p}$$

- **Constant $e$**: Indicates that the speedup is limited primarily by the **serial fraction** of the code (inherent algorithm limits).
- **Increasing $e$**: Indicates that **parallel overhead** (communication, synchronization, process startup) is the main bottleneck.

---

## Scalability Models

### Strong Scalability
- **Definition**: Keeping the **problem size fixed** while increasing the number of processes.
- **Goal**: Reduce total execution time.
- **Limitation**: Eventually limited by Amdahl's Law.

### Weak Scalability
- **Definition**: Increasing the **problem size proportionally** to the number of processes.
- **Goal**: Maintain constant efficiency ($E \approx 1$) as the problem grows.
- **Application**: Large-scale scientific simulations.
