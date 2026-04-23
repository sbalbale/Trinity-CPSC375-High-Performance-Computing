---
aliases: [Tuple Layout, Status Flag]
tags: [dbms, storage, architecture]
sources: [Homework 17.pdf, lec17.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Slot Structure

> [!abstract]
> The **Slot Structure** defines the physical layout of a single data unit (tuple) within a storage block. In the [[custom-dbms]], each block is divided into four 64-byte slots.

## Data Layout (64 Bytes)

1. **Status Flag (Byte 0)**:
    - `0`: The slot is **empty** (deleted or never used).
    - `1`: The slot contains a **valid** tuple.
2. **Tuple Data (Bytes 1-63)**:
    - Stores the actual attribute values (strings, integers) concatenated together.
    - Maximum string length is effectively 63 characters (including terminator).

## Connections
* **Prerequisites:** [[custom-dbms]].
* **Used In:** `dbput`, `dbget`, and `dbdelete` operations.
* **Contrasts With:** Variable-length records (not used in this simple system).
