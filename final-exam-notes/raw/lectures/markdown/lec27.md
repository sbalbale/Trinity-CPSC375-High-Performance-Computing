# Lecture 27: Performance Analysis

**Course:** CPSC 375: High-Performance Computing  
**Spring 2026** | Trinity College  
**Instructor:** Prof. Yoon

---

## Overview

This lecture covers fundamental principles of **parallel performance analysis**, including:
- Amdahl's Law (speedup with fixed problem size)
- Gustafson's Law (scaled speedup with growing problem size)
- Practical implications for parallel programming

---

## Key Concepts

### Speedup

The **speedup** is the ratio of serial execution time to parallel execution time:

$$S_p = \frac{T_s}{T_p}$$

Where:
- $T_s$ = Serial execution time
- $T_p$ = Parallel execution time on $p$ processors

---

## Amdahl's Law: The Pessimistic View

### Statement

Amdahl's Law describes the maximum speedup achievable when only a fraction of the program can be parallelized.

$$S_p = \frac{1}{(1-f) + \frac{f}{p}}$$

Where:
- $f$ = Fraction of the program that can be parallelized
- $p$ = Number of processors
- $(1-f)$ = Serial (non-parallelizable) fraction

### Derivation

If a program has:
- Serial execution time: $T_s$
- Parallelizable portion: $f \cdot T_s$ (runtime on $p$ processors: $\frac{f \cdot T_s}{p}$)
- Non-parallelizable portion: $(1-f) \cdot T_s$

Then:
$$T_p = (1-f) \cdot T_s + \frac{f \cdot T_s}{p}$$

$$S_p = \frac{T_s}{T_p} = \frac{T_s}{(1-f) \cdot T_s + \frac{f \cdot T_s}{p}} = \frac{1}{(1-f) + \frac{f}{p}}$$

---

## Example: Amdahl's Law in Practice

### Problem Setup

Suppose we can parallelize **90% of a serial program**:
- Original serial time: $T_s = 20$ seconds
- Parallelizable time: $0.9 \times T_s = 18$ seconds
- Serial/Fixed time: $0.1 \times T_s = 2$ seconds

### Parallel Execution with $p$ Processors

$$T_p = 0.9 \times T_s / p + 0.1 \times T_s = \frac{18}{p} + 2$$

### Speedup Calculation

$$S_p = \frac{T_s}{T_p} = \frac{20}{\frac{18}{p} + 2} = \frac{20 \cdot p}{18 + 2p}$$

### Results for Different Processor Counts

| $p$ (Processors) | $T_p$ (seconds) | $S_p$ (Speedup) |
|------------------|-----------------|-----------------|
| 1 | 20 | 1.0 |
| 2 | 11 | 1.82 |
| 4 | 6.5 | 3.08 |
| 8 | 4.25 | 4.71 |
| 16 | 3.125 | 6.40 |
| 32 | 2.625 | 7.62 |
| 64 | 2.3125 | 8.64 |
| 1000 | 2.018 | 9.91 |
| ∞ | 2 | **10.0** |

### Key Insight

**Speedup is bounded by $S_p \leq 10$**

Even with **1000 processors**, we can never achieve better than **10× speedup** when 90% of the program is parallelizable!

$$\lim_{p \to \infty} S_p = \frac{1}{1-f} = \frac{1}{0.1} = 10$$

### Generalization

$$\lim_{p \to \infty} S_p = \frac{1}{(1-f)}$$

The **serial fraction is the fundamental limit** on achievable speedup.

---

## Implications of Amdahl's Law

### The Pessimistic View

Amdahl's Law appears to suggest that parallel computing is limited by serial bottlenecks. If only 95% of a program can be parallelized:

$$\lim_{p \to \infty} S_p = \frac{1}{0.05} = 20$$

No matter how many processors you add, maximum speedup is 20×!

### Counter-Arguments

**Practical considerations:**

1. **Many scientific programs achieve excellent speedups**
   - Thousands of programs on supercomputers get speedups > 100×
   - Some get near-linear speedup with thousands of cores

2. **Improving serial portions**
   - Better algorithms can increase $f$
   - Optimization can reduce overhead

3. **Scalability improvements**
   - Reducing synchronization overhead
   - Better load balancing
   - Improved communication patterns

---

## Gustafson's Law: The Optimistic View

### The Problem with Amdahl's Law

Amdahl's Law assumes **fixed problem Size**.

In reality, when more processors become available, scientists and engineers solve **larger problems**, not the same problem faster.

### Gustafson's Law Statement

$$S_p = p + (1 - p) \cdot f$$

Where:
- $p$ = Number of processors
- $f$ = Serial fraction of the **scaled** problem on $p$ processors

### Interpretation

Instead of keeping problem size fixed:
- **Allocate more work** to the parallel portion
- **Keep execution time constant**
- Measure how much **more work** gets done

### Derivation

If we scale the problem so parallel work grows with $p$:
- Fixed component: $(1-f) \cdot T$ (serial work)
- Scaled component: $f \cdot p \cdot T$ (parallel work, increases with $p$)

On $p$ processors with constant time $T$:
$$S_p = \frac{\text{Total work on } p \text{ processors}}{\text{Work on 1 processor}} = \frac{(1-f) + f \cdot p}{(1-f) + f} = p + (1-p) \cdot f$$

---

## Example: Gustafson's Law in Practice

### Problem Setup

An application on **64 processors** requires 220 seconds.
Benchmarking shows **5% of time** is serial.

### Solution

Serial fraction: $f = 0.05$

$$S_{scaled} = 64 + (1 - 64) \times 0.05 = 64 - 3.15 = 60.85$$

### Interpretation

With 64 processors and only 5% overhead, we achieve **~61× speedup** for the scaled problem!

Much more optimistic than Amdahl's Law for the same scenario.

---

## Comparing the Laws

### Amdahl's Law (Fixed Problem Size)

| $p$ | 90% Parallelizable | 95% Parallelizable |
|-----|-------------------|-------------------|
| 10 | 5.26× | 6.67× |
| 100 | 9.17× | 16.81× |
| 1000 | 9.91× | 19.98× |
| ∞ | 10× | 20× |

**Speedup plateaus as $p$ increases.**

### Gustafson's Law (Scaled Problem)

| $p$ | 10% Serial | 5% Serial |
|-----|-----------|-----------|
| 10 | 9.1× | 9.55× |
| 100 | 91× | 95.95× |
| 1000 | 901× | 951× |
| ∞ | ∞ | ∞ |

**Speedup grows with $p$.**

---

## Which Law is Correct?

**Both are correct under different conditions:**

- **Amdahl's Law:** When you must complete a **fixed-size problem quickly** (e.g., real-time systems, fixed deadline)
- **Gustafson's Law:** When you want to **solve larger problems** with more resources (typical HPC usage)

### Real-World Practice

Most HPC applications follow **Gustafson's model**:
- Weather simulation: Increase grid resolution
- Molecular dynamics: More atoms/timesteps
- Machine learning: Larger datasets/models
- Climate modeling: Higher fidelity

When more processors are available, we **use them to solve harder problems**.

---

## Practical Considerations

### Why Not All Programs Scale Well

Even if $f$ is high, speedup is limited by:

1. **Communication overhead**
   - Time spent passing messages between processors
   - Network latency and bandwidth limitations

2. **Load imbalance**
   - Uneven work distribution
   - Some processors idle while others compute

3. **Synchronization overhead**
   - Barriers and locks
   - Waiting for slowest processor

4. **Memory contention**
   - Shared memory bottlenecks
   - Cache coherency traffic

### Amdahl's Law Revised

More realistically:
$$S_p = \frac{1}{(1-f) + \frac{f}{p} + \text{overhead}(p)}$$

Where **overhead** includes communication, synchronization, and load imbalance.

---

## Key Takeaways

| Concept | Key Points |
|---------|-----------|
| **Amdahl's Law** | Fixed problem, speedup limited by serial fraction |
| **Gustafson's Law** | Scaled problem, speedup grows with processors |
| **Speedup Bound** | Serial overhead is the fundamental limit |
| **Practical HPC** | Most systems follow Gustafson's model |
| **Optimization** | Minimize overhead and balance load |

---

## Questions for Reflection

1. If a program is 95% parallelizable, what is the theoretical maximum speedup?
2. How does increasing problem size affect Gustafson's Law?
3. Why do supercomputers achieve better speedups in practice than Amdahl's Law predicts?
4. What strategies can improve parallel scalability?

---

## Summary

- **Amdahl's Law** provides a pessimistic bound on speedup for fixed-size problems
- **Gustafson's Law** provides optimistic scaling for problems that grow with processor count
- Real-world HPC applications typically scale according to Gustafson's model
- Overhead (communication, synchronization, load imbalance) is often the real speedup limiter
- Understanding these laws is essential for designing efficient parallel algorithms
