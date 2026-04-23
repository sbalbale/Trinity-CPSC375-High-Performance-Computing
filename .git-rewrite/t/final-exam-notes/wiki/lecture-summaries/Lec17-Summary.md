---
aliases: [Performance Analysis Summary]
tags: [#lecture/detailed, #CPSC375]
sources: [lec17.pdf]
created: 2025-05-15
updated: 2025-05-15
---

# Lecture 17 Summary: Performance Analysis of Parallel Algorithms

> [!abstract] TL;DR
> Quantitative evaluation of parallel systems using **Speedup**, **Efficiency**, and **Scalability**. Contrasts **Amdahl's Law** (fixed problem size) with **Gustafson's Law** (scaled problem size) and introduces the **Isoefficiency Function**.

## 1. Primary Performance Metrics
* **Sequential Execution Time ($T_s$):** Time taken by the best known sequential algorithm on a single processor.
* **Parallel Execution Time ($T_p$):** Time taken by the parallel algorithm on $p$ processors.
* **Speedup ($S_p$):** $S_p = \frac{T_s}{T_p}$.
    * *Linear Speedup:* $S_p = p$.
    * *Superlinear Speedup:* $S_p > p$ (rare, often due to cache effects).
* **Efficiency ($E_p$):** $E_p = \frac{S_p}{p} = \frac{T_s}{p \cdot T_p}$.
    * Ideally $E_p = 1$. In practice, $E_p < 1$.

## 2. Amdahl's Law (Fixed Workload)
* **Context:** Problem size is fixed; we just add more processors.
* **Formula:** $S_p = \frac{1}{f + \frac{1-f}{p}}$
    * $f$: Fraction of the code that is inherently **sequential**.
    * $(1-f)$: Fraction that can be parallelized.
* **Implication:** As $p \to \infty$, $S_p \to \frac{1}{f}$. The sequential portion is the bottleneck.

## 3. Gustafson's Law (Scaled Workload)
* **Context:** As we add processors, we also increase the problem size (workload).
* **Formula:** $S_p = p + (1-p)f$
    * $f$: Fraction of time spent on sequential parts in the **scaled** problem.
* **Implication:** Speedup can increase linearly with $p$ if the problem size is allowed to grow. This is often more realistic for modern HPC.

## 4. Sources of Overhead ($T_o$)
* Total Overhead: $T_o = p \cdot T_p - T_s$.
* **Major Contributors:**
    1. **Inter-process Communication:** Sending data between nodes.
    2. **Synchronization:** Waiting at barriers.
    3. **Idleness:** Load imbalance.
    4. **Extra Computation:** Logic required for parallelization that wasn't in the sequential version.

## 5. Scalability and Isoefficiency
* **Scalability:** The ability of a parallel system to increase speedup in proportion to the number of processors.
* **Isoefficiency Function ($W = f(p)$):** Describes how the problem size ($W$) must increase as a function of $p$ to maintain a constant efficiency.
    * A system with a small isoefficiency function is **highly scalable**.
    * A system with a large isoefficiency function is **poorly scalable**.
