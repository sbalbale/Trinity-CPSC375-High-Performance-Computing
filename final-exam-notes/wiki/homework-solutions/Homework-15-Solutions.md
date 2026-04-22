---
aliases: [Homework 15 Solutions]
tags: [#homework/solutions, #course_hpc]
sources: [HW15_Solution.md, Homework 15.pdf]
created: 2026-04-20
updated: 2026-04-21
---

# Homework 15 Solutions

> [!abstract]
> Solutions to the parallel algorithm design exercises for Homework 15, focusing on domain decomposition, task mapping ratios, and agglomeration trade-offs.

## Problem 1: Weather Forecasting Model

**Scenario:** Design a parallel system for a weather model simulating atmospheric pressure, temperature, and wind speed across a 3D grid of the Earth.

### Domain Decomposition Approach
> [!equation] Data Partitioning
> **Domain decomposition** divides the primary data structure (the 3D grid) into pieces and associates the computation with each piece. The Earth's atmosphere is partitioned along longitude, latitude, and altitude boundaries to create **3D sub-volumes** (spatial blocks or "slabs").

**Data Division:**
Divide the grid into 3D sub-volumes. For example, partition the grid along longitude and latitude boundaries to create a checkerboard of vertical columns, or slice it horizontally by altitude layers. Each sub-volume holds local values for pressure, temperature, and wind speed.

**What a Primitive Task Represents:**
A **primitive task** represents the computation required to advance a **single grid cell** (one spatial point) forward by one time step. This includes reading its current state and applying physical equations (e.g., finite-difference approximations of fluid dynamics PDEs) to compute new values.

**Communication Pattern:**
Communication is **local**. Each task only needs values from its 6 immediate neighbors (up, down, north, south, east, west). Tasks at sub-volume boundaries must exchange "halo" values with adjacent sub-volumes at each time step.

---

## Problem 2: Mapping and Agglomeration

**Scenario:** Parallel algorithm with 1,000 primitive tasks running on a Lenovo P340 (8 physical cores, 16 logical processors).

### A. The 10:1 Ratio Rule
> [!question]
> If you decide to use static load balancing, what is the minimum number of tasks you should aim for to satisfy the "10:1 ratio" rule?

**Step-by-step Solution:**
1. Identify the number of processors ($p$). With 16 logical processors, $p = 16$.
2. Apply the **Mapping Checklist** rule: $\text{Target Tasks} \ge 10 \times p$.
3. $\text{Target Tasks} \ge 10 \times 16 = 160$.

**Answer:** You should aim for at least **160 tasks**. (The 1,000 tasks provided exceed this requirement).

### B. Agglomeration vs. Communication Overhead
> [!question]
> If the communication cost between the 1,000 tasks is high, would you prefer to keep tasks small or agglomerate them? Explain the trade-off.

**Answer:**
When communication cost is high, you should **agglomerate** the 1,000 tasks into a smaller number of larger tasks (ideally one per logical processor = 16 composite tasks).

**Trade-off Analysis:**
| Factor | Keep Tasks Small | Agglomerate into Larger Tasks |
|---|---|---|
| **Communication** | **High** — each boundary requires a message | **Low** — intra-task communication is eliminated |
| **Utilization** | High (fine-grained parallelism) | Risk of idle processors if tasks are uneven |
| **Locality** | Lower | **Higher** — computation occurs without cross-task messaging |

> [!warning] Agglomeration Preferred
> When communication is the bottleneck, agglomeration is preferred to maximize **locality**. By merging tasks that frequently communicate, that overhead becomes local memory access.
