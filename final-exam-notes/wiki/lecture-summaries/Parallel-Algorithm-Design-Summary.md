---
tags: [#lecture/detailed, #exam/algorithm_design]
sources: [lec15.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Parallel Algorithm Design — Lecture Summary

> [!abstract] Summary
> Detailed walkthrough of **Foster's Design Methodology** (PCAM) for creating efficient parallel algorithms. The methodology moves from fine-grained decomposition to physical mapping on processors.

## I. Introduction to Foster's Methodology
Foster's Methodology (PCAM) provides a structured four-stage process for parallel algorithm design:
1. **P**artitioning
2. **C**ommunication
3. **A**gglomeration
4. **M**apping

## II. Partitioning
The goal is to expose the maximum amount of parallelism by decomposing the problem into a large number of small tasks.
- **Domain Decomposition**: Focuses on partitioning the **data** (e.g., an array, a grid).
- **Functional Decomposition**: Focuses on partitioning the **computation** (e.g., different steps in a pipeline).
- **Checklist**:
    - Avoid redundant computation/data.
    - Ensure tasks are roughly the same size.
    - Tasks should increase with problem size.

## III. Communication
Defines the interaction patterns between tasks identified in the partitioning stage.
- **Types**:
    - **Local**: Task communicates with a small number of neighbors.
    - **Global**: Task communicates with many or all other tasks (e.g., reduction).
    - **Synchronous**: Blocking communication.
    - **Asynchronous**: Non-blocking communication.
- **Checklist**:
    - Minimize communication overhead.
    - Distribute communication evenly.
    - Overlap communication with computation where possible.

## IV. Agglomeration
Groups small tasks into larger "agglomerated" tasks to improve efficiency on real hardware.
- **Goals**:
    - Reduce communication costs (by making communication internal to a task).
    - Reduce task creation/management overhead.
    - Maintain enough parallelism for the target hardware.
- **Checklist**:
    - Does it improve performance?
    - Does it preserve scalability?

## V. Mapping
The final step is assigning agglomerated tasks to physical processors or threads.
- **Load Balancing**:
    - **Static**: Tasks assigned before execution (e.g., round-robin, block distribution).
    - **Dynamic**: Tasks assigned during execution (e.g., task queues, work stealing).
- **Goal**: Minimize total execution time by keeping all processors busy and minimizing inter-processor communication.
- **Checklist**:
    - Are processors utilized efficiently?
    - Is the mapping strategy scalable?
