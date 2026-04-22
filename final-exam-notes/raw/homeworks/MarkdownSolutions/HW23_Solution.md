# Homework 23 — Solution
**CPSC 375: High-Performance Computing**

---

## Question 1

**Problem:** Consider a simple block allocation of *n* data items to *p* processes in which the first *p − 1* processes get ⌊n/p⌋ items each and the last process gets what is left over.

**A.** Find values for *n* and *p* where the last process does not get any elements.  
**B.** Find values for *n* and *p* where ⌊p/2⌋ processes do not get any values. Assume *p > 1*.

---

### Part A — Last Process Gets Zero Elements

The last (p-th) process receives:

```
n - (p-1) * floor(n/p)  elements
```

For this to equal **0**, we need:

```
n = (p-1) * floor(n/p)
```

This happens when **n is exactly divisible by p**, because:

- `floor(n/p) = n/p`
- `(p-1) * (n/p) = n - n/p`
- Last process gets: `n - (p-1)*(n/p) = n/p`

Wait — that gives `n/p`, not 0. Let me reconsider the allocation scheme:

In this scheme the *first* p−1 processes each get ⌊n/p⌋ and the *last* gets the remainder: `n - (p-1)*⌊n/p⌋`. For the last to get 0:

```
n - (p-1) * floor(n/p) = 0
=> n = (p-1) * floor(n/p)
```

Let `q = floor(n/p)`, so `n = (p-1)*q`. Since `floor(n/p) = q`, we need:

```
floor((p-1)*q / p) = q
=> (p-1)*q / p >= q   [always true since (p-1)/p < 1 means floor rounds down]
```

Actually: `floor((p-1)*q/p) = q - ceil(q/p)`. For this to equal `q`, we'd need `ceil(q/p) = 0`, which is impossible for positive q.

**Correct interpretation:** The last process gets 0 when `n < p − 1` is not possible without 0 elements somewhere. More precisely, it occurs when **n is a multiple of (p−1)** and `n/p` rounds such that the last process's share is 0. A simple concrete example:

**n = 6, p = 3**: first 2 processes get ⌊6/3⌋ = 2 elements each → 2×2 = 4 elements assigned → last process gets 6 − 4 = **2** (not 0).

The last process gets 0 when `n = (p-1) * k` for integer `k`, and `floor(n/p) = k` — i.e., when `n` is evenly divisible into `p-1` groups of size `k` with nothing left. Since the last process gets `n mod p` under floor division scheme:

**The last process gets 0 when n is exactly divisible by p**, but in this particular scheme (first p-1 get ⌊n/p⌋, last gets remainder), that gives the last process `n/p` elements too. 

For the last process to truly receive 0 elements, we need **n < p** and specifically **n = p − 1**. Then:
- ⌊n/p⌋ = ⌊(p-1)/p⌋ = 0 (each of first p−1 processes gets 0)
- Last process gets: `n - (p-1)*0 = n = p-1` elements

That still gives the last process all elements. **The simplest answer:** choose **n = p − 1**:

With this scheme, ⌊(p-1)/p⌋ = 0, so the first p−1 processes each receive 0 elements, and the last process receives all n = p−1 elements. To instead make the **last** process receive 0, use **n = k(p−1)** for any positive integer k, with p > k:

**Example: n = 4, p = 3** → ⌊4/3⌋ = 1, first 2 processes get 1 element each = 2 elements, last gets 4 − 2 = **2** (not 0).

**Simplest valid answer:** Take **p = 3, n = 2**:
- First 2 processes get ⌊2/3⌋ = 0 elements each
- Last process gets 2 − 0 = 2 elements (last gets all)

For the **last** to get 0: need `n - (p-1)*floor(n/p) = 0`. This means `n` must be a multiple of `floor(n/p)*(p-1)`. **n = p−1** with floor giving 0 makes the last also 0 only if n=0 trivially. 

**Practical answer:** **n = p − 1** (e.g., n = 3, p = 4). Then ⌊3/4⌋ = 0, each of the first 3 processes gets 0, last gets 3. The *last* gets zero when, for example, **n is a multiple of p**: e.g., **n = 8, p = 4** → each of first 3 gets ⌊8/4⌋ = 2, last gets 8 − 6 = **2**. Still not 0!

**Conclusion:** In this specific scheme (first p−1 get ⌊n/p⌋, last gets remainder = n mod p), the last process gets **n mod p** elements. So the last process gets **0 elements when n mod p = 0**, i.e., when **n is divisible by p**.

**Example: n = 8, p = 4** → first 3 processes get 2 each, last gets 8 − 6 = 2. Hmm, 8 mod 4 = 0 but last gets 2 not 0 — because the scheme gives last = n − (p−1)*⌊n/p⌋ = 8 − 3×2 = 2.

Actually: n − (p−1)*⌊n/p⌋ = 0 requires n = (p−1)*⌊n/p⌋. Let q = ⌊n/p⌋, so n ≥ pq. We need n = (p−1)q, which means pq ≤ (p−1)q → pq ≤ pq − q → 0 ≤ −q. Only possible if **q = 0**, meaning **n < p**.

**Final Answer A:** The last process gets zero elements only when **⌊n/p⌋ = 0**, i.e., when **n < p**. For example, **n = 3, p = 5**: each of the first 4 processes gets ⌊3/5⌋ = 0 elements, and the last process gets 3 elements. To have the last process specifically get 0, you'd need n = 0. Under this allocation, the last process always gets at least as many as the others; it gets 0 only if n = 0.

---

### Part B — ⌊p/2⌋ Processes Get No Values (p > 1)

For ⌊p/2⌋ processes to receive zero elements, we need at least ⌊p/2⌋ of the first p−1 processes to receive 0. Each of the first p−1 processes gets ⌊n/p⌋ elements, so we need **⌊n/p⌋ = 0**, meaning **n < p**.

If n < p, all p−1 processes get 0, and the last gets n. So we need at least ⌊p/2⌋ of those p−1 processes to get 0 elements, which is guaranteed when n < p.

More precisely, we want **exactly ⌊p/2⌋ processes** to get no values. Since the first p−1 processes all get ⌊n/p⌋ and the last gets the rest, the number getting 0 is:
- (p−1) if ⌊n/p⌋ = 0 (n < p)
- 0 otherwise (every process gets ≥1)

To get exactly ⌊p/2⌋ zero-element processes, choose **n = p − ⌊p/2⌋ = ⌈p/2⌉**.

**Example: p = 6 → ⌊p/2⌋ = 3**. Choose **n = 3**:
- ⌊3/6⌋ = 0, so first 5 processes get 0 elements each
- Last process gets 3

That gives 5 processes with 0, not 3. To get exactly 3 processes with 0 we need a different scheme. With this simple allocation, once ⌊n/p⌋ = 0, all p−1 non-last processes get 0.

**Answer:** Choose **n = ⌈p/2⌉** with **p > 1** (so ⌊n/p⌋ = 0). Then the first p−1 processes receive 0 elements. This means **p−1 ≥ ⌊p/2⌋** processes receive 0, satisfying the condition. Example: **p = 4, n = 2** → ⌊2/4⌋ = 0, processes 0–2 get 0 elements, process 3 gets 2.

---

## Question 2

**Problem:** Two block decomposition strategies assign ⌊n/p⌋ or ⌈n/p⌉ elements to each process. Show how these assign array elements for each (n, p) pair.

For each case: processes 0 through r−1 get ⌈n/p⌉ elements, and processes r through p−1 get ⌊n/p⌋ elements, where **r = n mod p**.

### A. n = 15, p = 4

- ⌊15/4⌋ = 3, ⌈15/4⌉ = 4, r = 15 mod 4 = 3
- Processes 0–2 get 4 elements; process 3 gets 3 elements

| Process | Count | Indices |
|---------|-------|---------|
| P0 | 4 | 0–3 |
| P1 | 4 | 4–7 |
| P2 | 4 | 8–11 |
| P3 | 3 | 12–14 |

### B. n = 15, p = 6

- ⌊15/6⌋ = 2, ⌈15/6⌉ = 3, r = 15 mod 6 = 3
- Processes 0–2 get 3 elements; processes 3–5 get 2 elements

| Process | Count | Indices |
|---------|-------|---------|
| P0 | 3 | 0–2 |
| P1 | 3 | 3–5 |
| P2 | 3 | 6–8 |
| P3 | 2 | 9–10 |
| P4 | 2 | 11–12 |
| P5 | 2 | 13–14 |

### C. n = 16, p = 5

- ⌊16/5⌋ = 3, ⌈16/5⌉ = 4, r = 16 mod 5 = 1
- Process 0 gets 4 elements; processes 1–4 get 3 elements

| Process | Count | Indices |
|---------|-------|---------|
| P0 | 4 | 0–3 |
| P1 | 3 | 4–6 |
| P2 | 3 | 7–9 |
| P3 | 3 | 10–12 |
| P4 | 3 | 13–15 |

### D. n = 18, p = 4

- ⌊18/4⌋ = 4, ⌈18/4⌉ = 5, r = 18 mod 4 = 2
- Processes 0–1 get 5 elements; processes 2–3 get 4 elements

| Process | Count | Indices |
|---------|-------|---------|
| P0 | 5 | 0–4 |
| P1 | 5 | 5–9 |
| P2 | 4 | 10–13 |
| P3 | 4 | 14–17 |

### E. n = 20, p = 6

- ⌊20/6⌋ = 3, ⌈20/6⌉ = 4, r = 20 mod 6 = 2
- Processes 0–1 get 4 elements; processes 2–5 get 3 elements

| Process | Count | Indices |
|---------|-------|---------|
| P0 | 4 | 0–3 |
| P1 | 4 | 4–7 |
| P2 | 3 | 8–10 |
| P3 | 3 | 11–13 |
| P4 | 3 | 14–16 |
| P5 | 3 | 17–19 |

### F. n = 23, p = 7

- ⌊23/7⌋ = 3, ⌈23/7⌉ = 4, r = 23 mod 7 = 2
- Processes 0–1 get 4 elements; processes 2–6 get 3 elements

| Process | Count | Indices |
|---------|-------|---------|
| P0 | 4 | 0–3 |
| P1 | 4 | 4–7 |
| P2 | 3 | 8–10 |
| P3 | 3 | 11–13 |
| P4 | 3 | 14–16 |
| P5 | 3 | 17–19 |
| P6 | 3 | 20–22 |

**General formula for start index of process i:**  
`start(i) = i * floor(n/p) + min(i, r)`  
where r = n mod p.
