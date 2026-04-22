# Homework 15 — Solution
**CPSC 375: High-Performance Computing | Spring 2026**

---

## Problem 1

**You are designing a parallel system for a weather forecasting model that must simulate atmospheric pressure, temperature, and wind speed across a 3D grid of the Earth. Describe a domain decomposition approach for this problem. How would you divide the data, and what would a primitive task represent?**

### Solution

**Domain decomposition** divides the primary data structure into pieces and associates the computation with each piece.

**Dividing the data:**  
The Earth's atmosphere is modeled as a 3D grid with axes representing longitude, latitude, and altitude. Divide this grid into 3D sub-volumes (spatial blocks or "slabs"). For example, partition the grid along longitude and latitude boundaries to create a checkerboard of vertical columns, or slice it horizontally by altitude layers, or both. Each sub-volume holds local values for pressure, temperature, and wind speed at every grid point within it.

**What a primitive task represents:**  
A primitive task is the computation required to advance a single grid cell (one spatial point) forward by one time step — reading its current pressure, temperature, and wind speed, applying the governing physical equations (e.g., finite-difference approximations of the fluid dynamics PDEs), and computing new values. Because cells only depend on their immediate neighbors (local stencil), communication is **local**: each task only needs values from the 6 adjacent cells (up, down, north, south, east, west), making this a natural candidate for efficient domain decomposition.

**Communication pattern:**  
Tasks at sub-volume boundaries must exchange boundary ("halo") values with adjacent sub-volumes at each time step. Interior tasks communicate only within their sub-volume. This is classic local communication that scales well with problem size.

---

## Problem 2

**You have a parallel algorithm consisting of 1,000 primitive tasks. You plan to run this on one of the Lenovo P340 SFF compute nodes in your cluster (8 cores, 16 logical processors).**

### Part A — Minimum Tasks for the 10:1 Rule

**If you decide to use static load balancing, what is the minimum number of tasks you should aim for to satisfy the "10:1 ratio" rule mentioned in the Mapping Checklist?**

From the **Mapping Checklist** (Lecture 15): *"If static task allocation is chosen, the ratio of tasks to processors is at least 10:1."*

With 16 logical processors:

$$\text{Minimum tasks} = 10 \times 16 = \boxed{160 \text{ tasks}}$$

The algorithm already has 1,000 tasks, which gives a ratio of 1000/16 ≈ 62.5:1 — well above the minimum threshold of 10:1, satisfying the checklist requirement.

### Part B — Agglomeration vs. Communication Overhead

**If the communication cost between the 1,000 tasks is high, would you prefer to keep tasks small or agglomerate them? Explain the trade-off.**

When communication cost is high, you should **agglomerate** the 1,000 tasks into a smaller number of larger tasks (ideally one per logical processor = 16 composite tasks).

**Trade-off:**

| Factor | Keep Tasks Small | Agglomerate into Larger Tasks |
|---|---|---|
| Communication overhead | **High** — each inter-task boundary requires a message | **Low** — intra-task communication is eliminated |
| Processor utilization | High (fine-grained parallelism) | Risk of idle processors if tasks are uneven |
| Flexibility | Better load balancing options | Less flexible once agglomerated |
| Code complexity | Simpler task logic | More complex task logic |

When communication cost is the bottleneck, **agglomeration is preferred**. By merging tasks that frequently communicate into a single larger task, that communication becomes local memory access (free), dramatically reducing overhead. The goal is to maximize **locality** — the fraction of computation that occurs without cross-task communication. The Agglomeration Checklist (Lecture 15) confirms: "Locality of the parallel algorithm has increased" and "Agglomerated tasks have similar computational and communication costs."
