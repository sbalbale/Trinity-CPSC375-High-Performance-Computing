---
aliases: [Tuple Hashing, Key-to-Block Mapping]
tags: [dbms, storage, algorithm]
sources: [Homework 17.pdf, lec17.txt]
created: 2026-04-20
updated: 2026-04-20
---

# DBMS Hashing

> [!abstract]
> **DBMS Hashing** is the process used to determine the physical block index where a specific tuple should be stored. In the [[custom-dbms]], a simple sum-of-ASCII-values approach is used for primary key mapping.

## The Hash Function
> [!equation]
> $$h(\text{key}) = \left( \sum_{i=1}^{L} \text{ASCII}(\text{key}[i]) \right) \pmod{16}$$
> - **Range**: Maps keys to blocks 0-15 (base buckets).
> - **Input**: The string or integer representing the primary key.

> [!warning] Load Imbalance
> Simple modulo-based hashing can lead to "collisions" where many keys map to the same bucket while others remain empty, necessitating the use of [[overflow-blocks]].

## Connections
* **Prerequisites:** [[custom-dbms]].
* **Used In:** `dbput`, `dbget` operations.
* **Contrasts With:** B-Tree indexing (not used in this system).
