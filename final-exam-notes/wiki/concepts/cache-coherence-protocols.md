---
aliases: [Write Through, Write Once, Cache Consistency]
tags: [architecture, dsm, cache-coherence]
sources: [lec24.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Cache Coherence Protocols

> [!abstract]
> **Cache Coherence Protocols** are sets of rules that ensure all processors in a multi-processor system see a **consistent view** of memory. These are particularly critical in **Distributed Shared-Memory (DSM)** systems.

## Core Mechanics

### Write Through Protocol
- On every **Write Hit**, the local cache and main memory are updated simultaneously.
- Remote caches are either updated or invalidated.
- **Benefit**: Simplicity, as main memory is always current.
- **Drawback**: High bus traffic and low write performance.

### Write Once Protocol
A more efficient protocol using three main states:
1. **INVALID**: Cache data is stale or not present.
2. **CLEAN**: Cache matches main memory; data may be shared with other caches.
3. **DIRTY**: Cache contains the **only valid copy**; main memory is out-of-date.

> [!warning] Bus Snooping
> Most protocols rely on **bus snooping**, where every cache controller monitors the memory bus for read/write requests from other processors to maintain consistency.

## Implementations & Examples

> [!example] Write Once Workflow
> 1. CPU A reads word W: Memory provides W, marked as **CLEAN** in A.
> 2. CPU A writes word W: Memory is updated, others invalidated, marked as **CLEAN** in A.
> 3. CPU A writes word W again: No bus traffic, memory NOT updated, marked as **DIRTY** in A.
> 4. CPU B reads word W: A sees request, provides the data, memory is updated, both marked as **CLEAN**.

## Connections
* **Prerequisites:** [[distributed-shared-memory]].
* **Used In:** Symmetric Multi-Processing (SMP), NUMA systems.
* **Contrasts With:** Directory-based protocols (used in larger distributed systems).
