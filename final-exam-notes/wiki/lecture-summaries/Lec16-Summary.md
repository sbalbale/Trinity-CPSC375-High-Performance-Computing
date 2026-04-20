---
aliases: [More on Parallel Algorithm Design Summary]
tags: [#lecture/detailed, #CPSC375]
sources: [lec16.pdf]
created: 2025-05-15
updated: 2025-05-15
---

# Lecture 16 Summary: More on Parallel Algorithm Design

> [!abstract] TL;DR
> Deep dive into the **Mapping** stage of Foster's PCAM methodology. Distinguishes between **Static** and **Dynamic Mapping** and applies parallel design principles to **Parallel Search** and the **Sieve of Eratosthenes**.

## 1. Review of PCAM (Foster's Methodology)
* The lecture begins by reinforcing the four stages: Partitioning, Communication, Agglomeration, and Mapping.
* While the first three stages focus on the algorithm itself, **Mapping** is where the algorithm meets the hardware.

## 2. Mapping: Balancing the Scale
* **Objective:** Minimize total execution time.
* **Conflicting Constraints:**
    1. **Maximize Processor Utilization:** Keep all processors busy (load balancing).
    2. **Minimize Communication Costs:** Keep interacting tasks on the same or nearby processors.
* **Complexity:** Finding an optimal mapping is **NP-complete** for the general case. We rely on heuristics.

## 3. Static vs. Dynamic Mapping
### Static Mapping
* **Definition:** Tasks are assigned to processors *before* execution.
* **When to use:** Number of tasks and their computational costs are known and fixed.
* **Common Strategies:**
    * **Block Distribution:** Divide data into contiguous blocks.
    * **Cyclic Distribution:** Assign tasks in a round-robin fashion (better for uneven workloads if patterns are predictable).

### Dynamic Mapping
* **Definition:** Tasks are assigned to processors *during* execution.
* **When to use:** Task costs are unpredictable or tasks are generated on-the-fly.
* **The Master-Worker Pattern:**
    * A **Master** process manages a **Task Pool**.
    * **Worker** processes request tasks when they become idle.
    * **Trade-off:** High load balance but potential bottleneck at the Master process.

## 4. Example: Parallel Search
* Searching for a target value in a large dataset.
* **Strategy:** Partition the dataset, distribute to workers.
* **Early Termination:** If one worker finds the target, it must signal others to stop (Communication requirement).

## 5. Example: Sieve of Eratosthenes
* Finding all primes up to $n$.
* **Sequential Algorithm:** Iteratively mark multiples of the smallest un-marked prime.
* **Parallelization Challenges:**
    * Each step depends on the result of the previous (finding the next prime).
    * **Data Decomposition:** Partition the array of numbers.
    * **Broadcast:** The current prime must be communicated to all processors.
* **Optimization:** Only sieve multiples of primes up to $\sqrt{n}$.

## 6. Heuristic Approaches to Mapping
* For complex task dependency graphs, techniques like **Graph Partitioning** (e.g., using METIS) are used to group tasks that communicate heavily.
