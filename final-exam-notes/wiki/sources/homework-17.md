---
tags: [homework, dbms, storage, hashing, metadata]
sources: [Homework 17.pdf]
created: 2026-04-20
updated: 2026-04-20
---

# Homework 17

**Source:** Homework 17.pdf
**Date ingested:** 2026-04-20
**Type:** Homework Assignment

## Summary

This assignment explores the internal mechanics of a custom Relational Database Management System (DBMS). It covers the creation of metadata in the `catalog` and `columns` relations, the implementation of **Hash Files** for base relations (including ASCII-based hashing and overflow handling), buffer management strategies, and the differences between permanent **Base Relations** and temporary **Derived Relations**.

## Key Claims

- The system uses a fixed-block storage model (256 blocks of 256 bytes each).
- **Base Relations** use hash storage for fast lookup, while **Derived Relations** are stored as sequential heap files.
- Metadata is managed through a data dictionary consisting of the `catalog` and `columns` system tables.
- Data integrity is maintained by status flags in each 64-byte tuple slot and by using a **Bitmap (Block 0)** for disk allocation.

## Entities Mentioned

- [[custom-dbms]] — The internal system described in the coursework.

## Concepts Covered

- [[relation-metadata]] — Storage of table schemas.
- [[dbms-hashing]] — `sum(ASCII) mod 16` hash function.
- [[overflow-blocks]] — Handling full hash buckets.
- [[bitmap-allocation]] — Block 0 management.
- [[base-vs-derived-relations]] — Permanent vs. temporary storage.
- [[slot-structure]] — Flag and tuple layout.
