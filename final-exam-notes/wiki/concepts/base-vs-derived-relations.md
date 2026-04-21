---
aliases: [Heap Files vs Hash Files, Permanent vs Temporary Tables]
tags: [dbms, storage, architecture]
sources: [Homework 17.pdf, lec17.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Base vs. Derived Relations

> [!abstract]
> In relational database systems, **Base Relations** are permanent tables stored on disk, while **Derived Relations** are temporary tables generated as results of queries. They differ fundamentally in their storage structures and access methods.

## Comparison

| Feature | Base Relations | Derived Relations |
| :--- | :--- | :--- |
| **Storage Type** | **Hash Files** (random access) | **Heap Files** (sequential) |
| **Persistence** | Permanent (stored on disk) | Temporary (deleted after query) |
| **Key** | Defined Primary Key | Assume entire tuple is the key |
| **Catalog "Kind"**| `B` | `D` |

## Connections
* **Prerequisites:** [[custom-dbms]].
* **Used In:** Query optimization, relational algebra operations.
* **Contrasts With:** Virtual views (which are not materialized).
