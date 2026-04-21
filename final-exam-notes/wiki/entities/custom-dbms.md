---
aliases: [Course DBMS, Educational TPS]
tags: [#exam/entity, #dbms, #course_hpc]
sources: [Homework 17.pdf, lec17.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Custom DBMS

> [!abstract]
> The **Custom DBMS** is a pedagogical relational database management system implemented for CPSC 375. It features a fixed-block storage engine, ASCII-based hashing, and support for concurrent transaction processing.

## Architecture
- **Block Size**: 256 bytes.
- **Storage Limit**: 256 total blocks (64 KB simulated disk).
- **Slot Structure**: 64 bytes per tuple (max 4 tuples per block).
- **Metadata**: Stored in internal `catalog` and `columns` relations.

## Core Components
- **Buffer Manager**: Handles reading/writing blocks between RAM and the simulated disk file.
- **Hashing Engine**: Maps primary keys to block indices for [[base-vs-derived-relations]].
- **Lock Manager**: Ensures ACID properties during concurrent access.

## Connections
* **Architecture:** [[distributed-shared-memory]] (Assignment 7).
* **Properties:** [[acid-properties]].
* **Storage Logic:** [[bitmap-allocation]], [[overflow-blocks]].
