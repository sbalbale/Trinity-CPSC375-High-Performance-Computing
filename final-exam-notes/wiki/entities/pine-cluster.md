---
aliases: [Pine, Departmental Cluster]
tags: [#exam/entity, #CPSC375]
sources: [MPI Programming with Slurm.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Pine Cluster

> [!abstract]
> The **Pine Cluster** is the high-performance computing resource used for CPSC 375. It consists of 16 distributed-memory nodes connected by high-speed InfiniBand, managed by the Slurm workload manager.

## Hardware Specifications
- **Nodes**: 16 Dell PowerEdge R730.
- **Processors**: Dual Intel Xeon E5-2680 v3 (Haswell).
- **Cores**: 24 physical cores per node (48 logical with hyperthreading).
- **Memory**: 256GB DDR4 RDIMM per node.
- **Cache**: 60MB L3 cache per node.
- **Interconnect**: 40Gbps Mellanox InfiniBand.

## Connections
* **Managed By:** [[slurm]].
* **Used For:** Distributed [[mpi]] programming assignments.
* **Architecture:** [[distributed-memory-model]].
