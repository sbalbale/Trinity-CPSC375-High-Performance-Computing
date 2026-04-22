# Homework 17 — Solution
**CPSC 375: High-Performance Computing | Spring 2026**

---

## Problem 1

**You execute the following command in your DBMS:**
```
CR vehicles 3 1
vin S
make S
year I
```

### Part A — Catalog Entry

**List exactly what information will be added to the catalog relation for this new table.**

The `CR` command creates a base relation. One tuple is added to the **catalog** relation:

| Relname  | Kind | Attsize | Keysize | Relsize |
|----------|------|---------|---------|---------|
| vehicles | B    | 3       | 1       | 0       |

- **Relname** = `"vehicles"` — the name of the relation  
- **Kind** = `B` — base relation (created by user, stored permanently)  
- **Attsize** = `3` — number of attributes  
- **Keysize** = `1` — key occupies 1 attribute (the first attribute: `vin`)  
- **Relsize** = `0` — zero tuples inserted yet  

### Part B — Columns Entries

**How many tuples will be added to the columns relation? List Attname, Attdomain, and Attposition for each.**

**3 tuples** are added to the **columns** relation (one per attribute):

| Relname  | Attname | Attdomain | Attposition |
|----------|---------|-----------|-------------|
| vehicles | vin     | S         | 1           |
| vehicles | make    | S         | 2           |
| vehicles | year    | I         | 3           |

- `Attdomain` = `S` (string) or `I` (integer) as specified  
- `Attposition` reflects the order the attributes were declared  

### Part C — Why `DE catalog` Must Be Prevented

**Why must the system prevent a user from manually calling `DE catalog`?**

The **catalog** is the data dictionary — the master metadata table that records every relation in the database (its name, type, number of attributes, etc.). If a user could call `DE catalog`, they would destroy this entire metadata store, making every other relation in the database completely inaccessible (the system would no longer know those tables exist, their schemas, or where their data is stored on disk). It would effectively corrupt the entire DBMS. The catalog is a **system-owned, self-describing table** and must be protected from user modification to preserve database integrity.

---

## Problem 2

**You are inserting a tuple into the `vehicles` table with primary key `vin = "V101"`.**

### Part A — Hash Function Calculation

**Using the hash function for your DBMS, calculate the target bucket (data block number) for this tuple.**

Hash function: `h(key) = sum(ASCII(key)) mod 16`

Compute ASCII values for each character of `"V101"`:

| Character | ASCII |
|-----------|-------|
| V         | 86    |
| 1         | 49    |
| 0         | 48    |
| 1         | 49    |
| **Sum**   | **232** |

$$h(\text{"V101"}) = 232 \bmod 16 = \boxed{8}$$

Since $232 = 14 \times 16 + 8$, the target bucket (data block number) is **8**.

### Part B — Overflow Block Procedure

**If the target data block is already full (contains 4 tuples), describe the steps `dbput` must take to store the new tuple in an overflow block.**

1. **Read the target block** (block 8) from disk into the buffer and confirm it is full (all 4 slots occupied, each with flag = 1).
2. **Check for an existing overflow block**: examine the block header to see if block 8 already has an overflow block linked to it.
3. If no overflow block exists:  
   a. **Consult the Bitmap (Block 0)** to find the first free block (first bit = 0).  
   b. **Allocate that block**: flip its bit in the bitmap from 0 → 1 to mark it as used.  
   c. **Write the updated bitmap** back to disk.  
   d. **Update block 8's header** to point (link) to the new overflow block.
4. **Write the new tuple** into the first empty slot (slot flag = 0) of the overflow block.
5. **Write the overflow block** to disk.
6. **Flush the updated block 8 header** (with the overflow link) back to disk.

### Part C — Maximum Blocks for a Single Base Relation

**What is the maximum number of blocks a single base relation can occupy?**

The DBMS disk layout reserves 3 system blocks:
- Block 0: Bitmap  
- Block 1: Catalog header  
- Block 2: Columns header  

Total blocks = 256. System blocks = 3.

$$\text{Maximum blocks for a single relation} = 256 - 3 = \boxed{253 \text{ blocks}}$$

This would only occur if a single relation grew (through overflow chaining) to consume every available non-system block, which is the theoretical upper bound.

---

## Problem 3

**Your buffer currently holds 6 blocks: 1 Bitmap, 1 Catalog header, 1 Columns header, 3 Data blocks from a previous operation.**

### Part A — Buffer Slot for Vehicles Header

**If you need to read the header block for the vehicles relation, which of the 9 total allowed buffer slots should it occupy?**

The buffer has 9 slots (indexed 0–8). The current 6 blocks occupy slots 0–5. The vehicles relation header should be placed in **slot 6** — the first available free slot. This follows the buffer management policy of using the next available slot when reading a new block that is not already present in the buffer.

### Part B — Danger of Not Flushing a Dirty Block

**Explain the danger of not flushing a "dirty" data block to disk before reading a new block into the same buffer slot.**

A **dirty block** is a buffer block that has been modified in memory but whose changes have not yet been written back to disk. If the system evicts this dirty block to make room for a new block without first writing it to disk, all modifications made to that block are **permanently lost**. Any tuples inserted, updated, or deleted in that block — changes the user believes were committed — will simply disappear. The on-disk version will reflect the old state before those modifications, leading to **silent data corruption** and violation of durability. This is why the `dbput` / buffer management layer must always check the dirty flag and flush to disk before overwriting a buffer slot.

### Part C — How the Bitmap Helps Allocate New Blocks

**How does the Bitmap (Block 0) help determine where to allocate a new block when a relation grows?**

The Bitmap is a compact bit array where **each bit corresponds to one of the 256 blocks** on disk. A bit value of `0` means that block is **free**; a bit value of `1` means it is **occupied**. When a relation needs to grow (e.g., a full data block needs an overflow block), the system:

1. Reads the Bitmap into the buffer.  
2. Scans the bitmap to find the **first 0 bit** (first free block).  
3. Allocates that block by flipping its bit to `1`.  
4. Writes the updated Bitmap back to disk.  

This gives the system a fast, centralized, O(n) view of free space without needing to scan every block on disk.

---

## Problem 4

**You run a query that creates a result called `result_table`.**

### Part A — Kind Attribute in Catalog

**What will be the value of the Kind attribute for `vehicles` vs. `result_table`?**

| Relation      | Kind |
|---------------|------|
| vehicles      | `B` (base) |
| result_table  | `D` (derived) |

### Part B — Storage Structure Contrast

**Contrast the storage structure of `vehicles` (base) with `result_table` (derived).**

| Feature | `vehicles` (Base) | `result_table` (Derived) |
|---|---|---|
| Storage type | **Hash file** | **Heap file** |
| Access pattern | Random access via hash key | Sequential access only |
| Organization | 16 primary buckets (h(key) mod 16) + overflow chains | Unordered, tuples appended sequentially |
| Functions used | `dbput`, `dbget` (hash lookup) | `dbread`, `dbwrite` (sequential scan) |
| Persistence | Stored permanently on disk | Typically a temporary result |

### Part C — Why the Entire Tuple Is the Key for Derived Relations

**Why does the system assume the entire tuple is the key for derived relations?**

Derived relations are the result of relational operations (SELECT, PROJECT, JOIN, etc.) applied to base relations. Their output schema and content vary query-by-query — there is no user-declared primary key for a result set. By treating the **entire tuple as the key**, the system guarantees a unique identifier for every row in the result without needing to know anything about the relation's schema in advance. This simplifies the storage engine: there is no need to parse or hash a subset of columns, and uniqueness can always be determined by comparing the full row.

---

## Problem 5

**Each slot in your data blocks is 64 bytes.**

### Part A — Purpose of the Flag Byte

**What is the purpose of the first byte (the flag) in a slot?**

The flag byte is a **presence/occupancy indicator**:
- **`0`** = the slot is **empty** (available for a new tuple)  
- **`1`** = the slot is **occupied** (contains a valid tuple)  

This allows the system to efficiently scan blocks for free slots when inserting new tuples, and to skip empty slots when reading. It also serves as a simple delete marker — to delete a tuple, the system simply sets its flag to `0` without reorganizing the block.

### Part B — Maximum String Length

**If an attribute is a string, what is its maximum allowed length, and why?**

The maximum allowed string length is **63 bytes**.

Each slot is exactly 64 bytes. The **first byte is the flag**, leaving **63 bytes** for the tuple data. Since each slot stores exactly one tuple, and a string attribute occupies part or all of the data portion, a single string attribute is limited to 63 bytes (the full data payload of one slot).

### Part C — Two-Step Process to Change a Key

**Describe the two-step process required to change a key.**

Because the hash function `h(key) = sum(ASCII(key)) mod 16` maps a key to a specific data block, changing a tuple's key means it may belong in a **different block**. Simply overwriting in place would leave the tuple in the wrong hash bucket. Therefore:

1. **Delete the old tuple**: locate the current slot containing the tuple (using the old key to hash to the correct block), then set its flag byte from `1` → `0` (mark the slot as empty).  
2. **Re-insert the tuple with the new key**: compute `h(new_key)` to find the correct target block, then call `dbput` to insert the tuple with the updated key value into the proper hash bucket (handling overflow if needed).

This two-step delete-then-reinsert ensures the tuple always resides in the block determined by its current key.
