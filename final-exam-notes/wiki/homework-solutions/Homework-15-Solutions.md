---
aliases: [Homework 15 Solutions]
tags: [#homework/solutions, #course_hpc]
sources: [Homework 15.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Homework 15 Solutions

> [!abstract]
> Solutions to the parallel algorithm design exercises for Homework 15, focusing on domain decomposition, task mapping ratios, and agglomeration trade-offs.

## Problem 1: Weather Forecasting Model

**Scenario:** Design a parallel system for a weather model simulating atmospheric pressure, temperature, and wind speed across a 3D grid of the Earth.

### Domain Decomposition Approach
> [!equation] Data Partitioning
> The 3D grid (representing latitude, longitude, and altitude) would be divided into smaller **sub-blocks** or **3D sub-domains**. Each processor is responsible for the computation within its assigned volume.

**Data Division:**
The data would be divided using a **3D Block Decomposition**. The global volume is partitioned along all three axes $(x, y, z)$. For example, if the grid is $N \times N \times N$, and we have $p$ processors, we might divide it into sub-blocks of size $\frac{N}{\sqrt[3]{p}} \times \frac{N}{\sqrt[3]{p}} \times \frac{N}{\sqrt[3]{p}}$.

**What a Primitive Task Represents:**
A **primitive task** represents the calculations for a **single grid cell** (or a small atomic cluster of cells) in the 3D space. This task includes updating the pressure, temperature, and wind vectors for that specific point based on the values of its neighbors.

---

## Problem 2: Mapping and Agglomeration

**Scenario:** Parallel algorithm with 1,000 primitive tasks running on a Lenovo P340 (8 physical cores, 16 logical processors).

### A. The 10:1 Ratio Rule
> [!question]
> What is the minimum number of tasks you should aim for to satisfy the "10:1 ratio" rule for static load balancing?

**Step-by-step Solution:**
1. Identify the number of processors ($p$). For modern SFF nodes with hyperthreading, we use the number of logical processors.
2. $p = 16$.
3. Apply the rule: $\text{Target Tasks} = 10 \times p$.
4. $\text{Target Tasks} = 10 \times 16 = 160$.

**Answer:** You should aim for at least **160 tasks**.

### B. Communication Overhead vs. Processor Utilization
> [!question]
> If communication cost is high, would you prefer smaller tasks or agglomerated blocks? Explain the trade-off.

**Answer:**
If communication costs are high, you should **agglomerate primitive tasks into larger blocks**.

**Trade-off Explanation:**
- **Agglomeration (Larger Blocks):**
    - **Pros:** Reduces **communication overhead**. Grouping tasks reduces the total number of messages and increases the amount of computation per message.
    - **Cons:** Can decrease **processor utilization** and load balance. Having fewer, larger tasks makes it harder to ensure all processors finish at exactly the same time, potentially leaving some processors idle while the last large task completes.
- **Smaller Tasks:**
    - **Pros:** Better **load balancing** and higher potential parallelism.
    - **Cons:** Higher communication frequency, which may bottleneck the system if the interconnect latency is high.

> [!warning] Surface-to-Volume Ratio
> Agglomerating in 3D (forming cubes rather than slices) is generally more efficient because it minimizes the "surface area" (data to be communicated) relative to the "volume" (computational work).
