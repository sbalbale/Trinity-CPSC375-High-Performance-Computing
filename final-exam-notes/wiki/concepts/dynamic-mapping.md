---
aliases: [Runtime Mapping, Adaptive Mapping]
tags: [#exam/theory, #hpc]
sources: [lec16.pdf]
created: 2025-05-15
updated: 2025-05-15
---

# Dynamic Mapping

> [!abstract] TL;DR Summary
> Assignment of tasks to processors **during execution**. Essential for problems with **unpredictable** task costs or tasks generated dynamically.

## Core Mechanics
* **Task Pool:** A centralized or distributed repository of tasks.
* **Work Requesting:** Idle processors request new tasks (e.g., [[Master-Worker Pattern]]).
* **Work Stealing:** Idle processors "steal" tasks from the queues of busy processors.

> [!warning] Common Pitfalls
> * **Communication Overhead:** The logic to manage and distribute tasks can become a bottleneck or consume significant bandwidth.
> * **Master Bottleneck:** In a centralized scheme, the master process can limit scalability if it cannot keep up with worker requests.

## Connections
* **Antonym:** [[Static Mapping]]
* **Pattern:** [[Master-Worker Pattern]]
