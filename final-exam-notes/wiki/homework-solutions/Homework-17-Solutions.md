---
aliases: [Homework 17 Solutions]
tags: [#homework/solutions, #course_hpc, #dbms]
sources: [HW17_Solution.md, Homework 17.pdf]
created: 2026-04-20
updated: 2026-04-21
---

# Homework 17 Solutions

> [!abstract]
> Solutions to the DBMS internal implementation exercises for Homework 17, focusing on metadata management, hashing, storage structures, and buffer management.

## Problem 1: Relation Creation

**Command:** `CR vehicles 3 1` (`vin S`, `make S`, `year I`)

### A. Catalog Entry
> [!info]
> One tuple is added to the **catalog** relation (Base relation):
> | Relname | Kind | Attsize | Keysize | Relsize |
> |---|---|---|---|---|
> | `vehicles` | `B` | `3` | `1` | `0` |

### B. Columns Entries
**3 tuples** are added to the **columns** relation:
| Relname | Attname | Attdomain | Attposition |
|---|---|---|---|
| `vehicles` | `vin` | `S` | `1` |
| `vehicles` | `make` | `S` | `2` |
| `vehicles` | `year` | `I` | `3` |

### C. Preventing `DE catalog`
**Why prevent manual deletion?**
The `catalog` is a **system-owned, self-describing table**. Deleting from it would destroy the metadata store, rendering every other relation inaccessible. It would corrupt the entire DBMS.

---

## Problem 2: Hashing and Overflow

**Inserting tuple with primary key `vin = "V101"`.**

### A. Hash Calculation
> [!equation] Hash Function
> $$h(\text{key}) = \left( \sum \text{ASCII}(\text{key}) \right) \pmod{16}$$
> 
> ASCII('V')=86, ASCII('1')=49, ASCII('0')=48, ASCII('1')=49.
> **Sum** = 232.
> $232 \pmod{16} = \boxed{8}$

### B. Overflow Block Procedure
If the target block (8) is full:
1. **Consult Bitmap (Block 0)** to find the first free block (first bit = 0).
2. **Allocate**: Flip bit to 1 and write bitmap back to disk.
3. **Link**: Update Block 8's header to point to the new overflow block.
4. **Write**: Store the tuple in the first empty slot of the overflow block.

### C. Maximum Blocks
Total blocks = 256. System blocks (Bitmap, Catalog, Columns) = 3.
**Maximum blocks for one relation** = $256 - 3 = \boxed{253}$.

---

## Problem 3: Buffer Management

### A. Buffer Slot
The buffer has 9 slots (0-8). If 6 blocks occupy slots 0-5, the `vehicles` header should occupy **slot 6** (first available).

### B. Dirty Block Risk
> [!warning] Durability Violation
> If a **dirty block** (modified in memory) is evicted without flushing to disk, all changes (inserts/updates) are **permanently lost**. This leads to silent data corruption.

### C. Bitmap (Block 0)
The Bitmap is a compact bit array where each bit corresponds to a block (0=free, 1=occupied). It provides a fast, centralized view of free space for allocation.

---

## Problem 4: Storage Contrast

### A. Kind Attribute
- `vehicles`: `B` (Base)
- `result_table`: `D` (Derived)

### B. Storage Structure
| Feature | Base (`vehicles`) | Derived (`result_table`) |
|---|---|---|
| **Storage** | **Hash file** | **Heap file** |
| **Access** | Random (via hash key) | Sequential scan |
| **Persistence** | Permanent | Temporary |

---

## Problem 5: Data Layout

### A. Purpose of Flag Byte
The flag is an **occupancy indicator**: `0` = empty, `1` = occupied. It allows for efficient insertion and simple deletion.

### B. Max String Length
Slot size = 64 bytes. 1 byte used for flag.
**Max string length** = **63 bytes**.

### C. Changing a Key
1. **Delete** the old tuple (locate via `h(old_key)` and set flag to 0).
2. **Re-insert** with the new key (compute `h(new_key)` and call `dbput`).
> [!info]
> This ensures the tuple resides in the correct block determined by its current key.
