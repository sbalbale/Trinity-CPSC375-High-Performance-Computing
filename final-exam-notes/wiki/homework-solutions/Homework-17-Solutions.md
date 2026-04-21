---
aliases: [Homework 17 Solutions]
tags: [#homework/solutions, #course_hpc, #dbms]
sources: [Homework 17.pdf, lec17.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Homework 17 Solutions

> [!abstract]
> Solutions to the DBMS internal implementation exercises for Homework 17, focusing on metadata management, hashing, storage structures, and buffer management.

## Problem 1: Relation Creation

**Command:** `CR vehicles 3 1` (Attributes: `vin S`, `make S`, `year I`)

### A. Information added to the `catalog` relation:
> [!info]
> - **Relname:** `vehicles`
> - **Kind:** `B` (Base relation)
> - **Attsize:** `3` (Number of attributes)
> - **Keysize:** `1` (Primary key consists of the first attribute)
> - **Relsize:** `0` (Initial number of tuples)

### B. Tuples added to the `columns` relation:
Three tuples will be added, one for each attribute:
| Relname | Attname | Attdomain | Attposition |
| :--- | :--- | :--- | :--- |
| `vehicles` | `vin` | `S` | `0` |
| `vehicles` | `make` | `S` | `1` |
| `vehicles` | `year` | `I` | `2` |

### C. Manual Deletion from Catalog
**Why prevent `DE catalog`?**
The system must prevent manual deletion from system relations like `catalog` because it would lead to **metadata inconsistency**. The catalog is the ground truth for where data is stored on disk; if a user deletes a entry manually, the physical data blocks associated with that table would become "orphaned" and inaccessible, while the [[bitmap-allocation]] would still show them as occupied.

---

## Problem 2: Hashing and Overflow

**Inserting tuple with primary key `vin = "v101"`.**

### A. Hash Calculation
> [!equation] Hash Function
> $$h(\text{key}) = \left( \sum \text{ASCII}(\text{key}) \right) \pmod{16}$$

**Step-by-step:**
1. ASCII('v') = 118
2. ASCII('1') = 49
3. ASCII('0') = 48
4. ASCII('1') = 49
5. Sum = $118 + 49 + 48 + 49 = 264$
6. $264 \pmod{16} = 8$ (since $16 \times 16 = 256$, and $264 - 256 = 8$)

**Answer:** The target bucket is **Block 8**.

### B. Overflow Handling
If Block 8 is full (contains 4 tuples), the `dbput` function performs these steps:
1. Consult the **Bitmap (Block 0)** to find the first available free block.
2. Mark that block as occupied in the bitmap.
3. Link the current full block to this new **overflow block** (by updating a pointer in the block's header).
4. Write the new tuple into the first slot of the newly allocated overflow block.

---

## Problem 3: Buffer Management

### A. Header Slot
If the buffer holds 9 slots, the header block for `vehicles` should occupy a slot designated for data blocks (typically slots 3-8 if 0, 1, 2 are reserved for Bitmap, Catalog, and Columns).

### B. The "Dirty" Block Risk
**Danger of not flushing:**
If a "dirty" block (a block that has been modified in memory) is not flushed to the simulated Linux disk file before being overwritten by a new block, the **updates are permanently lost**. The memory state would differ from the disk state, violating the **Durability** property of the system.

### C. Bitmap (Block 0) Role
The Bitmap tracks the status of all 256 blocks. Each bit (or byte in some implementations) represents a block. When a relation needs to grow, the system scans the bitmap for a `0` (free), changes it to a `1` (used), and uses that block index for the new data.

---

## Problem 4: Base vs. Derived Relations

### A. Kind Attribute
- **vehicles:** `B` (Base)
- **result_table:** `D` (Derived)

### B. Storage Contrast
- **Base Relations:** Use **Hash Files**. Data is placed in buckets based on a key for $O(1)$ lookup.
- **Derived Relations:** Use **Heap Files**. Data is stored sequentially (appended) for $O(n)$ sequential reading.

### C. Derived Relation Key Assumption
The system assumes the **entire tuple is the key** for derived relations because they are often the result of operations (like Projection or Union) that may remove the original primary key or combine data in ways where a single attribute is no longer unique.

---

## Problem 5: Data Layout

### A. Purpose of First Byte
The first byte is a **status flag**:
- `0`: The slot is **empty**.
- `1`: The slot contains a **valid tuple**.

### B. Max String Length
In a 64-byte slot:
- 1 byte is used for the flag.
- 63 bytes remain.
**Answer:** The maximum length is **63 characters** (including the null terminator).

### C. Changing a Key
Changing a primary key is a two-step process because the key determines the physical location (bucket) of the tuple:
1. **Delete** the existing tuple from its current bucket.
2. **Re-insert** the tuple with the new key so the hash function can place it in the correct new bucket.
