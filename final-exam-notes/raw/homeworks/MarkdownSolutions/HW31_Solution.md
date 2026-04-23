# Homework 31 — Solutions

**Course:** CPSC 375 – High-Performance Computing (Spring 2026) **Topic:** Parallel matrix multiplication — block-striped, Cannon's, Fox's, and SUMMA

---

## Problem 1

### Restated

The scalability of parallel matrix multiplication is often limited by the ratio of computations per communication. In the block-striped algorithm, each process undergoes _p − 1_ block shifts. Using the complexity analysis from slide 16, derive why the block-striped ratio of _n/p_ is considered poor compared to Cannon's ratio of _n/√p_.

### Solution

**Block-striped complexity (from slide 16).** Each process owns a stripe of _A_, _B_, and _C_ of size (_n/p_) × _n_. The algorithm runs _p_ iterations; in each iteration a process multiplies an (_n/p_) × (_n/p_) block of _A_ by an (_n/p_) × _n_ block of _B_:

- Work per iteration: O(n³ / p²)
- Total computation over _p_ iterations: **T_comp = O(n³ / p)**

On the communication side, every process must eventually see all _p_ stripes of _B_. It already holds one, so it receives the other _p − 1_ by shifting them around a ring. Each stripe is (_n/p_) · _n_ = _n²/p_ elements, and each process forwards (_p − 1_) of them:

- Total communication per process: (_p − 1_) · _n²/p_ ≈ **T_comm = O(n²)**

**Block-striped ratio:** $$ \frac{T_{\text{comp}}}{T_{\text{comm}}} = \frac{n^3 / p}{n^2} = \frac{n}{p} $$

**Cannon's complexity.** Cannon's uses a √p × √p checkerboard. Each process owns an (_n/√p_) × (_n/√p_) block of _A_, _B_, and _C_. There are √p shift-multiply iterations. Per iteration a process multiplies two (_n/√p_) × (_n/√p_) blocks:

- Work per iteration: O(n³ / p^(3/2))
- Total: √p · n³ / p^(3/2) = **T_comp = O(n³ / p)** (same total work — good)

Communication per iteration is two blocks of size (_n/√p_)² = _n²/p_, so over √p iterations:

- Total: √p · n²/p = **T_comm = O(n² / √p)**

**Cannon's ratio:** $$ \frac{T_{\text{comp}}}{T_{\text{comm}}} = \frac{n^3 / p}{n^2 / \sqrt{p}} = \frac{n}{\sqrt{p}} $$

### Why _n/p_ is poor compared to _n/√p_

Fix a scalability target — say we need the ratio to stay above some constant _R_ so that computation hides communication. Then:

|Algorithm|Required relation|Interpretation|
|---|---|---|
|Block-striped|_n ≥ R · p_|_n_ must scale **linearly** with _p_|
|Cannon's|_n ≥ R · √p_|_n_ only needs to scale as **√p**|

Two equivalent ways to see why block-striped is worse:

1. **Total communication volume per process.** In block-striped, every process must see the entire matrix _B_ (_n²_ elements), because its _C_-stripe depends on every column of _B_. Cannon's local _C_-block only depends on _n/√p_ rows of _A_ and _n/√p_ columns of _B_, which is only _n²/√p_ elements. Cannon's cuts per-process traffic by a factor of **√p**.
    
2. **Ratio decays with _p_.** As the processor count grows, block-striped's ratio _n/p_ collapses √p times faster than Cannon's _n/√p_. For example, at _p_ = 10 000 with _n_ = 10 000:
    
    - Block-striped: _n/p_ = 1 → one flop per element communicated (terrible, communication-bound).
    - Cannon's: _n/√p_ = 100 → 100 flops per element communicated (much healthier).

So the poor _n/p_ ratio comes from the fact that a 1-D stripe decomposition forces each process to communicate _O(n²)_ data regardless of _p_, whereas Cannon's 2-D checkerboard caps per-process communication at _O(n²/√p)_.

---

## Problem 2 — Cannon's and Fox's on a √p × √p checkerboard

### Part A — Restated

In Cannon's algorithm, block _A_{ij}_ is cycled left _i_ positions and block _B_{ij}_ is cycled up _j_ positions during initial alignment. Trace the specific movement of blocks _A_{1,2}_ and _B_{1,2}_ in a 4 × 4 processor mesh.

#### Solution

Here √p = 4, so all shifts wrap modulo 4.

**Block A_{1,2}:**

- Initial location: processor P_{1,2} (row 1, column 2).
- Row 1 of _A_ is cycled left by _i_ = 1 position.
- New column index: (2 − 1) mod 4 = **1**.
- **A_{1,2} moves from P_{1,2} → P_{1,1}** (one hop west along row 1).

**Block B_{1,2}:**

- Initial location: processor P_{1,2}.
- Column 2 of _B_ is cycled up by _j_ = 2 positions.
- New row index: (1 − 2) mod 4 = **3**.
- **B_{1,2} moves from P_{1,2} → P_{3,2}** (two hops north with wrap-around to the bottom of column 2).

After alignment, the pair sitting together at P_{1,1} is (_A_{1,2}_, _B_{2,1}_), and the pair at P_{3,2} is (_A_{3,1}_, _B_{1,2}_) — exactly the kind of "same-color" matched pairs the alignment is designed to produce so that the first multiply step is immediately useful.

---

### Part B — Restated

Compare the memory overhead of Fox's algorithm, which requires an extra buffer to store the broadcasted _A_ block, against Cannon's algorithm, which is strictly in-place regarding communication buffers.

#### Solution

**Cannon's.** Each process owns one _A_-block and one _B_-block of size (_n/√p_)². Every step it sends its block to a neighbor and overwrites it with the incoming block from the other neighbor. No extra buffer is required — the send and receive slots are the same memory. Per-process steady-state storage:

$$ M_{\text{Cannon}} = \underbrace{\frac{n^2}{p}}_{A_{ij}} + \underbrace{\frac{n^2}{p}}_{B_{ij}} + \underbrace{\frac{n^2}{p}}_{C_{ij}} = \frac{3n^2}{p} $$

**Fox's.** The broadcasted _A_-block is received _from another processor in the same row_, but each processor still has to hold onto its _own_ original _A_-block because it may be the broadcaster in a later step. That forces a second buffer for the incoming _A_:

$$ M_{\text{Fox}} = \underbrace{\frac{n^2}{p}}_{A_{ij} \text{ (own)}} + \underbrace{\frac{n^2}{p}}_{A_{\text{recv buffer}}} + \underbrace{\frac{n^2}{p}}_{B_{ij}} + \underbrace{\frac{n^2}{p}}_{C_{ij}} = \frac{4n^2}{p} $$

**Overhead:** Fox's needs roughly **one extra (_n/√p_) × (_n/√p_) block per process — about _n²/p_ additional elements, a ~33 % memory increase** over Cannon's. In memory-bound regimes (very large _n_, limited RAM per node, or GPU HBM budgets), this can tip the algorithm choice toward Cannon's.

---

### Part C — Restated

Identify a hardware scenario where `MPI_Bcast` optimization might make Fox's algorithm faster than the manual shifts used in Cannon's.

#### Solution

Any platform where the MPI collective is implemented on top of hardware that scales sub-linearly with process count — most notably:

- **Clusters with hardware multicast or tree-based broadcast**, e.g., InfiniBand networks with hardware multicast support, Cray Aries / Slingshot, or HPE Cray EX systems. On these, `MPI_Bcast` achieves roughly **O(log √p)** latency along a row, versus the √p − 1 sequential point-to-point hops of Cannon's ring shift.
- **GPU clusters using NCCL / RCCL** for the collective. NCCL broadcasts exploit NVLink/NVSwitch topology and deliver near-peak bandwidth with log-tree algorithms, whereas Cannon's shift is a chain of send/recv pairs along a logical mesh and cannot fully exploit the fat links.
- **Shared-memory / single-node NUMA systems** where `MPI_Bcast` collapses to a memcpy from one shared cache line to many readers, while Cannon's still pays the overhead of √p − 1 discrete message exchanges.

In all three cases the row-broadcast in Fox's step completes in _O(log √p · n²/p)_ time using a tree algorithm, beating Cannon's _O(√p · n²/p)_ sum of serial shifts on the same hardware — so Fox's wins despite the extra buffer.

---

## Problem 3 — SUMMA

### Part A — Restated

Explain how rewriting the matrix product into a sum of outer products $C = \sum A_{,k} \times B_{k,}$ allows SUMMA to be more flexible than algorithms requiring a perfect square number of processors.

#### Solution

Cannon's and Fox's both require a √p × √p grid, so _p_ must be a **perfect square** (_p_ = 4, 9, 16, 25, …). If you only have 12 or 20 processes, you either waste nodes or fall back to a sub-optimal mapping.

The outer-product formulation breaks that constraint. Notice what each step actually requires:

- _A_{_,k}* is a **single column panel** of _A_ (width _b_, height _n_). It only needs to travel **along the rows** of the processor grid.
- _B_{k,_}* is a **single row panel** of _B_ (height _b_, width _n_). It only needs to travel **along the columns** of the processor grid.
- The local update _C_{local} ← C_{local} + A_{panel} · B_{panel}_ at each processor depends only on the panels it receives — not on whether its neighbors form a square mesh.

Because the two broadcasts are **independent** (one along rows, one along columns) and neither requires the mesh to be square, SUMMA maps cleanly onto **any** _p_r_ × _p_c_ grid with _p_r · p_c = p_. You can use 2 × 6, 3 × 4, 4 × 5, etc., and the algorithm is identical. The same flexibility extends to **non-square matrices** _C_ = _A_ · _B_ of size _m_ × _n_, since the outer-product index _k_ is decoupled from the output dimensions.

In short: Cannon's couples the algorithm structure to a square grid; SUMMA's outer-product view makes the row-broadcast and column-broadcast the only structural requirements, and those work on any rectangular grid.

---

### Part B — Restated

In SUMMA the block size _b_ is chosen to balance communication latency and computational throughput. Explain how a very small _b_ might increase latency overhead while a very large _b_ might reduce the efficiency of pipelining.

#### Solution

SUMMA performs _n/b_ outer steps, and each step issues a row-broadcast of an _A_-panel, a column-broadcast of a _B_-panel, and a local rank-_b_ update.

**Very small _b_ (e.g., _b_ = 1 or 2).**

- Number of steps = _n/b_, which is huge. Total latency cost ≈ (_n/b_) · α per process, where α is the per-message latency. Latency **dominates** the runtime.
- The local update is a rank-_b_ GEMM. With _b_ ≈ 1 it degenerates to a rank-1 update (essentially BLAS-2 / `dger`), which is memory-bound and only reaches a small fraction of peak FLOPS. You lose the BLAS-3 cache-reuse benefit that makes GEMM fast in the first place.

**Very large _b_ (e.g., _b_ on the order of _n/√p_).**

- Each broadcast moves a lot of data in one shot: (_n/√p_) · _b_ elements per panel. Per-step cost is dominated by bandwidth term β · (volume).
- **Pipelining suffers.** A well-tuned SUMMA overlaps the next step's broadcast with the current step's local GEMM. Large _b_ means the local GEMM is long, so the next broadcast — even if issued early — has no more compute to hide behind. Equivalently, the number of pipeline stages _n/b_ becomes tiny, so you can't keep the network busy while compute runs.
- Memory footprint for the panel buffers grows linearly with _b_, crowding out the _C_ block in cache.

**Sweet spot.** Typical SUMMA implementations pick _b_ in the **32 – 128** range. That's large enough that the inner GEMM is BLAS-3 and hits near-peak FLOPS, while small enough that (i) latency per step is amortized, (ii) there are enough _n/b_ stages to pipeline communication with computation, and (iii) the panel buffers stay in L2/L3 cache.