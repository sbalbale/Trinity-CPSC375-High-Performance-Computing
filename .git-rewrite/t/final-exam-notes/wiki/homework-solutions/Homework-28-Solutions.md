---
aliases: [Homework 28 Solutions]
tags: [#homework/solutions, #course_hpc, #performance, #sorting]
sources: [Homework 28.pdf, lec28.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Homework 28 Solutions

> [!abstract]
> Solutions to the isoefficiency derivation and bitonic sorting exercises for Homework 28, featuring scalability analysis and a step-by-step sorting trace.

## Problem 1: Isoefficiency Derivation

**Goal:** Derive the relationship between work ($W$) and overhead ($T_o$) to maintain constant efficiency ($E$).

**Proof:**
1. Start with the definition of Parallel Efficiency:
   $$E = \frac{T_s}{p T_p}$$
2. Substitute sequential work $W = T_s$ and the model for parallel time $T_p = \frac{W}{p} + T_o(p)$:
   $$E = \frac{W}{p \left( \frac{W}{p} + T_o(p) \right)}$$
3. Simplify the denominator:
   $$E = \frac{W}{W + p T_o(p)}$$
4. Rearrange to solve for $W$:
   $$E(W + p T_o(p)) = W$$
   $$EW + E p T_o(p) = W$$
   $$E p T_o(p) = W - EW$$
   $$E p T_o(p) = W(1 - E)$$
5. Isolate $W$:
   $$W = \frac{E}{1 - E} p T_o(p)$$
6. Let $K = \frac{E}{1 - E}$ (a constant for a fixed desired efficiency):
   $$W = K \cdot p T_o(p)$$

---

## Problem 2: Scalability Analysis

**Scenario:** $W = n$ (sequential work) and $T_o = p \sqrt{n}$ (communication overhead).

### A. Deriving the Isoefficiency Function
$$W = K \cdot p T_o \implies n = K \cdot p (p \sqrt{n})$$
$$n = K p^2 \sqrt{n}$$
$$\frac{n}{\sqrt{n}} = K p^2 \implies \sqrt{n} = K p^2$$
$$n = K^2 p^4$$

**Isoefficiency Function:** **$O(p^4)$**

### B. Categorization and Growth
- **Categorization:** This algorithm is **poorly scalable**. An isoefficiency of $O(p^4)$ means that the problem size must grow at a massive rate just to keep additional processors busy.
- **Doubling Processors:** If $p$ is doubled ($p \to 2p$):
  $$n_{new} = K^2 (2p)^4 = 16 K^2 p^4 = 16 n_{old}$$
  You must increase the problem size $n$ by a factor of **16**.

---

## Problem 3: Bitonic Sorting Trace

**Unordered Sequence:** $[8, 3, 4, 7, 9, 2, 1, 5]$

### A. Phase-by-Phase Trace
1. **Phase 1 (Length 2 Bitonic Sequences):**
   - (8,3) $\to$ [3, 8] (Inc)
   - (4,7) $\to$ [7, 4] (Dec)
   - (9,2) $\to$ [2, 9] (Inc)
   - (1,5) $\to$ [5, 1] (Dec)
   - **Result:** $[3, 8, 7, 4, 2, 9, 5, 1]$

2. **Phase 2 (Length 4 Bitonic Sequences):**
   - Block 1 [3, 8, 7, 4] $\to$ Inc: $[3, 4, 7, 8]$
   - Block 2 [2, 9, 5, 1] $\to$ Dec: $[9, 5, 2, 1]$
   - **Result:** $[3, 4, 7, 8, 9, 5, 2, 1]$ (This is now a single **bitonic sequence** of length 8)

3. **Phase 3 (Length 8 Bitonic Sequence):**
   - Step 1 (Split): (3,9), (4,5), (7,2), (8,1) $\to$ [3, 4, 2, 1] and [9, 5, 7, 8]
   - Step 2: [2, 1, 3, 4] and [7, 5, 9, 8]
   - Step 3: **Result:** $[1, 2, 3, 4, 5, 7, 8, 9]$

### B. Total Compare-and-Exchange Steps
For $n=8$ ($k=3$):
- Each comparison step uses $n/2 = 4$ operations.
- Total steps $= 1 + 2 + 3 = 6$ parallel steps.
- **Total C&E operations:** $6 \times 4 = \mathbf{24}$ operations.

### C. State after first Bitonic Split
When the bitonic sequence $[3, 4, 7, 8, 9, 5, 2, 1]$ undergoes its first four comparisons (distance 4):
1. (3, 9) $\to$ 3 remains at index 0, 9 remains at index 4.
2. (4, 5) $\to$ 4 remains at index 1, 5 remains at index 5.
3. (7, 2) $\to$ **Swap**: 2 moves to index 2, 7 moves to index 6.
4. (8, 1) $\to$ **Swap**: 1 moves to index 3, 8 moves to index 7.

**Resulting state:** Two sequences $[3, 4, 2, 1]$ and $[9, 5, 7, 8]$. 
**Key Property Observed:** Every element in the first sequence is smaller than every element in the second sequence.
