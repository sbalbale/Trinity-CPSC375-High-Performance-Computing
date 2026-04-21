---
aliases: [Practice Questions, Flashcards, Mock Exam]
tags: [#exam/prep, #course_hpc]
sources: [exam2_study_guide.html]
created: 2026-04-20
updated: 2026-04-20
---

# Exam 2: Self-Assessment

> [!abstract]
> A collection of flashcards and multiple-choice questions curated from the official study guide to test your understanding of Lectures 15–31.

## Flashcards

> [!example] 1. What does the reduction clause do in OpenMP?
> **Answer**: It creates private copies of a variable for each thread, performs the operation locally, then combines all copies into a single result at the end of the parallel region. Eliminates race conditions on accumulation operations.

> [!example] 2. Difference between `#pragma omp critical` and `#pragma omp atomic`?
> **Answer**: `critical` serializes a whole block of code (any size); `atomic` applies to a single memory update (read-modify-write). `atomic` is typically faster because it can use hardware instructions.

> [!example] 3. State Amdahl's Law and its implication.
> **Answer**: $S_p = 1 / (f + (1−f)/p)$, where $f$ is the serial fraction. As $p \to \infty$, speedup approaches $1/f$. Implication: even a small serial fraction severely limits maximum speedup.

> [!example] 4. How does Gustafson's Law differ from Amdahl's Law?
> **Answer**: Amdahl assumes fixed problem size (strong scaling) — pessimistic. Gustafson assumes problem size grows with $p$ (weak scaling) — optimistic.

> [!example] 5. What does the Karp–Flatt metric reveal?
> **Answer**: $e = (1/S_p − 1/p)/(1 − 1/p)$. It captures the empirically-observed serial fraction, including all real overheads like communication latency and synchronization.

> [!example] 6. What is `MPI_Allgather`?
> **Answer**: It collects data from all processes and delivers the concatenated result to every process (not just root).

> [!example] 7. Domain decomposition for Sieve?
> **Answer**: Array split into contiguous blocks across $p$ processes. Primes up to $\sqrt{n}$ are used to sieve the local subarray.

> [!example] 8. Three Sieve optimizations?
> **Answer**: 1. Eliminate even integers. 2. Eliminate broadcast (local sequential sieve for primes up to $\sqrt{n}$). 3. Reorganize loops for cache efficiency (blocking).

> [!example] 9. How does Odd-Even Sort work in parallel?
> **Answer**: $n$ alternating phases (Odd/Even). Adjacent rank pairs compare-and-swap.

> [!example] 10. Describe Shearsort on a grid.
> **Answer**: Alternate row sorts (snake order) and column sorts (standard order). Converges in $\lceil \log_2 p \rceil + 1$ iterations.

> [!example] 11. Cannon's vs. Fox's?
> **Answer**: Cannon uses initial skewing and shift-multiply (in-place). Fox uses row-broadcast and column-shift (needs extra buffer).

> [!example] 12. What is a race condition?
> **Answer**: Unpredictable output due to concurrent access to shared data without synchronization.

> [!example] 13. Difference between `MPI_Scatter` and `MPI_Bcast`?
> **Answer**: `Bcast` sends same data to everyone. `Scatter` distributes distinct portions of an array.

> [!example] 14. What is a thread pool?
> **Answer**: A fixed set of pre-created threads that pick up tasks from a queue to avoid creation/destruction overhead.

> [!example] 15. What does `#pragma omp barrier` do?
> **Answer**: Forces all threads in the parallel region to wait until every thread has reached that point.

> [!example] 16. Computation-to-communication ratio: Block-Striped vs. Cannon's?
> **Answer**: Block-striped: $\approx n/p$. Cannon's: $\approx n/\sqrt{p}$. Cannon's scales better.

> [!example] 17. What is `#pragma omp single`?
> **Answer**: Only one thread executes the block; others wait at an implicit barrier at the end.

> [!example] 18. Strong vs. Weak scaling?
> **Answer**: Strong: fixed problem size, increase $p$. Weak: problem size grows with $p$.

> [!example] 19. What is tiling/blocking?
> **Answer**: Reordering loops so a submatrix (tile) fits in cache, increasing cache hit rate.

> [!example] 20. What does the `nowait` clause do?
> **Answer**: Removes the implicit barrier at the end of a `for` or `sections` construct.

## Practice Multiple Choice

1. **Which OpenMP clause ensures per-thread copies are combined at the end?**
   - A. `private`
   - B. `shared`
   - C. **`reduction`**
   - D. `critical`
   *Explanation: `reduction` combines results with an operator; `private` only isolates them.*

2. **What scheduling policy assigns iterations to threads on demand?**
   - A. `static`
   - B. **`dynamic`**
   - C. `guided`
   - D. `nowait`
   *Explanation: `dynamic` uses a task pool approach for load balancing.*

3. **Which operation sends distinct portions of an array from root to all processes?**
   - A. `MPI_Bcast`
   - B. `MPI_Reduce`
   - C. **`MPI_Scatter`**
   - D. `MPI_Gather`
   *Explanation: `Scatter` divides; `Bcast` replicates.*

4. **Amdahl's Law: If 10% of a program is serial, max speedup is?**
   - A. **10**
   - B. 9
   - C. 5
   - D. No limit
   *Explanation: $1/0.10 = 10$.*

5. **Karp-Flatt: If $e$ increases as $p$ grows, what does it indicate?**
   - A. Serial fraction decreasing
   - B. **Overhead (comm/sync) is significant**
   - C. Gustafson's Law applies
   - D. Perfectly parallel
   *Explanation: Increasing $e$ means $T_o$ (overhead) is the bottleneck.*

6. **Which law assumes problem size scales with $p$?**
   - A. Amdahl's Law
   - B. Karp-Flatt
   - C. **Gustafson's Law**
   - D. Moore's Law
   *Explanation: Gustafson defines "scaled speedup."*

7. **How many phases does Odd-Even sort require for $n$ elements?**
   - A. $\log n$
   - B. $n/2$
   - C. **$n$**
   - D. $2n$
   *Explanation: Requires $n$ alternating phases to guarantee sorted order.*

8. **Cannon's primary advantage over block-striped?**
   - A. Uses fewer processes
   - B. **Better computation-to-communication ratio**
   - C. No initial alignment
   - D. Works only on squares
   *Explanation: Cannon achieves $n/\sqrt{p}$ ratio.*

9. **What does `#pragma omp barrier` do?**
   - A. Creates critical section
   - B. **Waits for all threads to reach this point**
   - C. Removes overhead
   - D. Static assignment
   *Explanation: It is a synchronization wait-point.*

10. **What does `MPI_Allgather` do?**
    - A. Sends to root only
    - B. Broadcasts one value
    - C. **Concatenated buffers to all processes**
    - D. Reduces to one process
    *Explanation: Dual of `Gather` (all receive result).*

11. **Sieve: Eliminating `MPI_Bcast` requires each process to?**
    - A. Use different algorithm
    - B. **Run local sequential sieve for primes up to $\sqrt{n}$**
    - C. Share subarrays
    - D. Only mark 2 and 3
    *Explanation: Local computation replaces network communication.*

12. **What distinguishes Fox's from Cannon's?**
    - A. 1D decomposition
    - B. **Broadcasts A blocks along rows**
    - C. No data movement
    - D. Uses `Scatter` for B
    *Explanation: Fox uses broadcasts; Cannon uses shifts.*
