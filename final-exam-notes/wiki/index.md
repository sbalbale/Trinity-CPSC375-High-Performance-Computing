# Wiki Index

## Sources
- [[getting-started-with-openmp-programming]] — Lab manual introducing OpenMP, parallel regions, and data-sharing.
- [[lec15]] — Parallel Algorithm Design using Foster's Methodology.
- [[lec16]] — Deep dive into Mapping and parallelization examples (Search, Sieve).
- [[lec17]] — Performance analysis metrics (Speedup, Efficiency, Amdahl's Law).
- [[lec21]] — Collective Communication.
- [[lec22]] — Derived Types and Barriers.
- [[lec23]] — Parallel Sieve of Eratosthenes.

## Entities
- [[openmp]] — Open Multi-Processing API for shared-memory multiprocessing.
- [[fosters-methodology]] — Four-stage framework (PCAM) for designing parallel algorithms.
- [[sieve-of-eratosthenes]] — Classic prime-finding algorithm used for parallel analysis.
- [[mpi]] — Message Passing Interface, used for distributed memory parallelism.

## Concepts
- [[parallel-region]] — Block of code executed by multiple threads simultaneously.
- [[race-condition]] — Non-deterministic behavior caused by concurrent access to shared data.
- [[reduction-operation]] — Efficiently combines private variable copies into a single value.
- [[data-sharing-clauses]] — Mechanisms to control variable scoping in OpenMP.
- [[partitioning]] — Decomposing a problem into fine-grained tasks.
- [[communication]] — Defining data exchange patterns between tasks.
- [[agglomeration]] — Grouping tasks to reduce overhead and improve efficiency.
- [[mapping-parallel-programming]] — Assigning tasks to physical processors.
- [[domain-decomposition]] — Data-centric partitioning strategy.
- [[functional-decomposition]] — Task-centric partitioning strategy.
- [[load-balancing]] — Strategies to minimize processor idle time.
- [[static-mapping]] — Pre-runtime task assignment (Block/Cyclic).
- [[dynamic-mapping]] — Runtime task assignment for unpredictable loads.
- [[master-worker-pattern]] — Common dynamic mapping strategy using a task pool.
- [[parallel-speedup]] — Ratio of sequential to parallel execution time.
- [[parallel-efficiency]] — Metric for processor utilization ($S_p/p$).
- [[amdahls-law]] — Speedup limits for fixed problem sizes.
- [[gustafsons-law]] — Speedup perspective for scaled problem sizes.
- [[isoefficiency-function]] — Metric for evaluating parallel system scalability.
- [[overhead-parallel-computing]] — Non-computational time (comm, sync, idle).
- [[collective-communication]] — Communication involving all processes in a communicator.
- [[data-partitioning]] — Strategies for dividing data among processes.
- [[block-decomposition]] — Strategy for dividing data into contiguous chunks.
- [[interleaved-decomposition]] — Cyclic data distribution.
- [[local-vs-global-indices]] — Mapping indices in parallel arrays.
- [[mpi-derived-datatypes]] — Representing collections of data items in memory.
- [[mpi-barrier]] — Collective synchronization across processes.
- [[performance-measurement-mpi]] — Using `MPI_Wtime` for profiling.
- [[acid-properties]] — Core requirements for database transactions.

## Exam Prep
## Homework Solutions
## Lecture Summaries
- [[Parallel Algorithm Design-Summary]] — Detailed walkthrough of Foster's Methodology (lec15).
- [[Lec16-Summary]] — Mapping strategies and Parallel Sieve/Search (lec16).
- [[Lec17-Summary]] — Performance metrics and scaling laws (lec17).
- [[lec21-Summary]] — MPI Collective Communication operations.
- [[lec22-Summary]] — MPI Derived Types, Barriers, and ACID properties.
- [[lec23-Summary]] — Parallel Sieve implementation and block decomposition.
## Synthesis
