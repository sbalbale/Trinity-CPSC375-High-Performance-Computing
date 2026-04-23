---
aliases: [DBMS Internals, Relational DB Sheet]
tags: [#exam/theory, #prep/moc, #course_hpc]
sources: [Homework-17-Solutions.md, lec17.md]
created: 2026-04-20
updated: 2026-04-20
---

# Cheat Sheet: DBMS Internals

> [!abstract]
> High-density summary of custom DBMS architecture, metadata, and storage logic.

## 1. Physical Architecture
| Component | Specification |
| :--- | :--- |
| **Simulated Disk** | 256 Blocks |
| **Block Size** | 256 Bytes |
| **Slot Size** | 64 Bytes |
| **Tuples / Block** | Max 4 |

## 2. Slot Layout
> [!code] 64-Byte Slot
> - **Byte 0:** Status Flag (`0` = empty, `1` = valid).
> - **Bytes 1-63:** Tuple Attributes (Concatenated).

## 3. Storage Structures
| Structure | Usage | Access Pattern |
| :--- | :--- | :--- |
| **Hash Files** | Base Relations | Random (via Primary Key) |
| **Heap Files** | Derived Relations | Sequential (Append-only) |

## 4. Metadata (Data Dictionary)
> [!info]
> - **Catalog Table:** Relname, Kind (B/D), Attsize, Keysize, Relsize.
> - **Columns Table:** Relname, Attname, Attdomain (S/I), Attposition.

## 5. Storage Algorithms
> [!equation] DBMS Hashing
> $$h(\text{key}) = \text{sum(ASCII)} \pmod{16}$$
> - Result maps to base buckets (Blocks 0-15).

> [!info] Bitmap (Block 0)
> Used to track block allocation status across the disk.

> [!info] Overflow Blocks
> Secondary blocks linked to full buckets to handle collisions.

## 6. DBMS Operations
> [!code]
> - **dbput:** Hash key $\to$ find block $\to$ scan for empty slot $\to$ write.
> - **dbget:** Hash key $\to$ search block and overflow chain for matching key.
> - **dbdelete:** Set status flag to `0`.
