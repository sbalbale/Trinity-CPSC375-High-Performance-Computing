# Homework 28 — Solution
**CPSC 375: High-Performance Computing**

---

## Question 1 — Isoefficiency Function Derivation

**Problem:** The isoefficiency function characterizes the rate at which problem size W must increase relative to the number of processors p to maintain constant efficiency. Derive the isoefficiency metric.

Hint: Start from E = T_1 / (p · T_p), model T_p = W/p + T_o(p), substitute, simplify to E = W / (W + p · T_o(p)), then solve for W assuming E is constant.

---

### Derivation

**Step 1: Define Efficiency**

Efficiency E is defined as speedup per processor:

```
E = S_p / p = T_1 / (p · T_p)
```

where T_1 is sequential execution time = W (total work).

**Step 2: Model T_p**

The parallel execution time consists of useful computation plus overhead:

```
T_p = W/p + T_o(p)
```

where:
- W/p = computation time (work divided evenly across p processors)
- T_o(p) = total overhead per processor (communication, synchronization, idle time)

**Step 3: Substitute into Efficiency Formula**

```
E = W / (p · T_p)
  = W / (p · [W/p + T_o(p)])
  = W / (W + p · T_o(p))
```

**Step 4: Solve for W (Isoefficiency Function)**

Fix E = constant C (the desired efficiency level, e.g., 0.5):

```
C = W / (W + p · T_o(p))
C · (W + p · T_o(p)) = W
C · W + C · p · T_o(p) = W
C · p · T_o(p) = W - C · W
C · p · T_o(p) = W(1 - C)
W = C / (1 - C) · p · T_o(p)
```

Since C/(1−C) is a constant (for fixed target efficiency), the isoefficiency function is:

$$\boxed{W = \frac{C}{1 - C} \cdot p \cdot T_o(p)}$$

Or written more concisely (absorbing the constant into K):

$$W \geq K \cdot p \cdot T_o(p)$$

**Interpretation:** To maintain constant efficiency as p increases, the problem size W must grow at least as fast as p · T_o(p). The isoefficiency function tells us how scalable an algorithm is:

| T_o(p) form | W must grow as | Scalability |
|-------------|----------------|-------------|
| O(1)        | O(p)           | Highly scalable |
| O(log p)    | O(p log p)     | Scalable |
| O(p)        | O(p²)          | Moderately scalable |
| O(p²)       | O(p³)          | Poorly scalable |

---

## Question 2 — Isoefficiency for T_o = p√n

**Problem:** A parallel algorithm has sequential work W (number of operations). Its total communication overhead is T_o = p√n, where n is the problem size and p is the number of processors. Derive the isoefficiency function, categorize the algorithm's scalability, and determine by what factor you must increase n when doubling p to maintain constant efficiency.

---

### Step 1: Express W in Terms of n

The sequential work is W = W(n) (some function of problem size n). For this derivation, we treat W and n as related — in many problems W = n or W = n log n, but here we use W directly in the isoefficiency condition.

### Step 2: Apply the Isoefficiency Condition

From Question 1, we need:

```
W ≥ K · p · T_o(p)
```

Substituting T_o = p√n:

```
W ≥ K · p · (p√n)
W ≥ K · p² · √n
```

### Step 3: Express as Isoefficiency Function

Since W is the sequential work and in many contexts W = n (or we treat problem size as W), substitute W = n:

```
n ≥ K · p² · √n
n / √n ≥ K · p²
√n ≥ K · p²
n ≥ K² · p⁴
```

**Isoefficiency function: W = O(p⁴)** (or equivalently, n must grow as p⁴).

### Step 4: Categorize Scalability

Since W must grow as p⁴ to maintain efficiency:
- This is a **polynomial (quartic) growth requirement** — the problem must grow very fast relative to p.
- This is classified as **poorly scalable** (or moderately-to-poorly scalable).

To put it in perspective:
- Highly scalable: W grows as O(p) or O(p log p)
- Moderately scalable: W grows as O(p²)
- Poorly scalable: W grows as O(p³) or faster

An isoefficiency of O(p⁴) means this algorithm does **not scale well** — doubling processors requires a 16× increase in problem size just to maintain the same efficiency.

### Step 5: Factor by Which n Must Increase When p Doubles

From the isoefficiency relation n = K · p⁴:

If p → 2p:

```
n_new = K · (2p)⁴ = K · 16p⁴ = 16 · n_old
```

**You must increase n by a factor of 16 when doubling the number of processors.**

This confirms the poor scalability: every time you double p, the problem size must grow 16× to keep efficiency constant.

---

## Question 3 — Bitonic Sort on [8, 3, 4, 7, 9, 2, 1, 5]

**Problem:** Given the unordered sequence [8, 3, 4, 7, 9, 2, 1, 5]:

**A.** Sort the integers into a single increasing list using Bitonic Sorting phases.  
**B.** Calculate the exact number of total compare-and-exchange steps required to sort 8 numbers.  
**C.** If the sequence is currently bitonic, describe the state of the two resulting sequences after the first four compare-and-exchange operations.

---

### Background: Bitonic Sort Structure

For n = 8 = 2³, Bitonic Sort has **log₂(8) = 3 stages**, with the k-th stage containing k steps. Total steps = 1 + 2 + 3 = **6 compare-and-exchange steps** (each step operates on all n/2 = 4 pairs simultaneously).

Initial sequence (0-indexed positions):
```
Index:  0   1   2   3   4   5   6   7
Value:  8   3   4   7   9   2   1   5
```

---

### Part A — Bitonic Sort Trace

The algorithm builds a sorted sequence by:
1. Creating bitonic sequences of increasing size
2. Merging them into a fully sorted sequence

**Stage 1 (step 1): Sort pairs into alternating ↑↓ sequences**

Compare-and-exchange pairs: (0,1), (2,3), (4,5), (6,7)
- Pair (0,1): 8,3 → sorted ↑: min first → **3, 8**
- Pair (2,3): 4,7 → sorted ↓: max first → **7, 4**
- Pair (4,5): 9,2 → sorted ↑: min first → **2, 9**
- Pair (6,7): 1,5 → sorted ↓: max first → **5, 1**

After Stage 1:
```
Index:  0   1   2   3   4   5   6   7
Value:  3   8   7   4   2   9   5   1
```
Bitonic subsequences: [3,8] ↑, [7,4] ↓, [2,9] ↑, [5,1] ↓

---

**Stage 2:**

*Step 2a*: Compare pairs with stride 2 within groups of 4
Groups: (0..3) sorted ↑, (4..7) sorted ↓
- Group 0..3 ↑: compare (0,2) and (1,3)
  - (0,2): min(3,7)=3 → pos 0; max(3,7)=7 → pos 2
  - (1,3): min(8,4)=4 → pos 1; max(8,4)=8 → pos 3
- Group 4..7 ↓: compare (4,6) and (5,7)
  - (4,6): max(2,5)=5 → pos 4; min(2,5)=2 → pos 6
  - (5,7): max(9,1)=9 → pos 5; min(9,1)=1 → pos 7

After step 2a:
```
Index:  0   1   2   3   4   5   6   7
Value:  3   4   7   8   5   9   2   1
```

*Step 2b*: Compare pairs with stride 1 within groups of 4
- Group 0..3 ↑: compare (0,1) and (2,3)
  - (0,1): min(3,4)=3, max=4 → 3, 4
  - (2,3): min(7,8)=7, max=8 → 7, 8
- Group 4..7 ↓: compare (4,5) and (6,7)
  - (4,5): max(5,9)=9, min=5 → 9, 5
  - (6,7): max(2,1)=2, min=1 → 2, 1

After Stage 2:
```
Index:  0   1   2   3   4   5   6   7
Value:  3   4   7   8   9   5   2   1
```
Now [3,4,7,8] is sorted ↑ and [9,5,2,1] is sorted ↓ → together form a bitonic sequence.

---

**Stage 3:**

*Step 3a*: Stride 4 across full array (↑ direction)
- Compare (0,4): min(3,9)=3 → pos 0; max=9 → pos 4
- Compare (1,5): min(4,5)=4 → pos 1; max=5 → pos 5
- Compare (2,6): min(7,2)=2 → pos 2; max=7 → pos 6
- Compare (3,7): min(8,1)=1 → pos 3; max=8 → pos 7

After step 3a:
```
Index:  0   1   2   3   4   5   6   7
Value:  3   4   2   1   9   5   7   8
```

*Step 3b*: Stride 2
- Compare (0,2): min(3,2)=2, max=3 → 2, 3
- Compare (1,3): min(4,1)=1, max=4 → 1, 4
- Compare (4,6): min(9,7)=7, max=9 → 7, 9  ← wait, sort ↑ globally
  - Actually still merging ↑: min(9,7)=7 → pos 4, max=9 → pos 6... 

Let me redo stage 3 cleanly using the standard bitonic merge on the full array for ↑ order:

After Stage 2 we have bitonic sequence: [3,4,7,8,9,5,2,1]

**Step 3a**: stride=4, compare-swap for ↑ (keep smaller in lower index)
- (0,4): 3 vs 9 → 3,_,_,_,9,_,_,_  (3<9, no swap)
- (1,5): 4 vs 5 → no swap (4<5)
- (2,6): 7 vs 2 → swap → 2 at pos 2, 7 at pos 6
- (3,7): 8 vs 1 → swap → 1 at pos 3, 8 at pos 7

After step 3a: `3, 4, 2, 1, 9, 5, 7, 8`

**Step 3b**: stride=2, compare-swap ↑
- (0,2): 3 vs 2 → swap → 2, _, 3, _
- (1,3): 4 vs 1 → swap → _, 1, _, 4
- (4,6): 9 vs 7 → swap → _, _, _, _, 7, _, 9, _
- (5,7): 5 vs 8 → no swap

After step 3b: `2, 1, 3, 4, 7, 5, 9, 8`

**Step 3c**: stride=1, compare-swap ↑
- (0,1): 2 vs 1 → swap → 1, 2
- (2,3): 3 vs 4 → no swap → 3, 4
- (4,5): 7 vs 5 → swap → 5, 7
- (6,7): 9 vs 8 → swap → 8, 9

After step 3c: `1, 2, 3, 4, 5, 7, 8, 9`

**Final sorted sequence: [1, 2, 3, 4, 5, 7, 8, 9]** ✓

---

### Part B — Total Compare-and-Exchange Steps

For n = 8 = 2³ elements, Bitonic Sort requires:

```
Total steps = Σ(k=1 to log₂n) k = Σ(k=1 to 3) k = 1 + 2 + 3 = 6
```

Each step performs n/2 = 4 simultaneous compare-and-exchange operations.

**Total compare-and-exchange operations = 6 steps × 4 pairs = 24**

More generally, for n elements:

```
Total parallel steps   = (log₂n)(log₂n + 1) / 2
Total individual C&E   = (n/2) · (log₂n)(log₂n + 1) / 2
```

For n = 8: (8/2) · (3)(4)/2 = 4 · 6 = **24 compare-and-exchange operations** across **6 parallel steps**.

---

### Part C — State After First Four Compare-and-Exchange Operations on a Bitonic Sequence

A **bitonic sequence** first increases then decreases (or vice versa). Using our sequence after Stage 2: `[3, 4, 7, 8, 9, 5, 2, 1]`, which is bitonic (increases to 9, then decreases).

**First four compare-and-exchange operations** = Step 3a (stride-4 pass), which performs exactly 4 C&E operations:

| Operation | Positions | Values | Result |
|-----------|-----------|--------|--------|
| C&E 1 | (0,4) | 3 vs 9 | 3 stays, 9 stays (no swap) |
| C&E 2 | (1,5) | 4 vs 5 | 4 stays, 5 stays (no swap) |
| C&E 3 | (2,6) | 7 vs 2 | swap → 2 at [2], 7 at [6] |
| C&E 4 | (3,7) | 8 vs 1 | swap → 1 at [3], 8 at [7] |

**State of two resulting sequences after the first four C&E operations:**

The stride-4 pass splits the 8 elements into two groups of 4:

- **Lower half** (positions 0–3): `[3, 4, 2, 1]`
- **Upper half** (positions 4–7): `[9, 5, 7, 8]`

**Properties of the resulting sequences:**
- The lower half `[3, 4, 2, 1]` contains all the **smaller elements** (each ≤ corresponding upper-half element), but is not yet sorted.
- The upper half `[9, 5, 7, 8]` contains all the **larger elements**, also not yet sorted.
- Both halves are themselves **bitonic sequences**: [3,4,2,1] increases then decreases; [9,5,7,8] decreases then increases.
- The subsequent steps (3b and 3c) will independently sort each half using smaller strides, ultimately producing the fully sorted array.

This is the key invariant of bitonic merge: **one stride-halving pass on a bitonic sequence of length n produces two bitonic subsequences of length n/2, each containing either all the smaller or all the larger elements.**
