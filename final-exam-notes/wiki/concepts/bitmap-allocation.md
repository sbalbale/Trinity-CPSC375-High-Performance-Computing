---
aliases: [Block Allocation, Free Space Map]
tags: [dbms, storage, architecture]
sources: [Homework 17.pdf, lec17.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Bitmap Allocation

> [!abstract]
> **Bitmap Allocation** is a technique for managing free and occupied blocks on a disk. It uses a bit (or byte) for every physical block to indicate whether it is currently used by a relation or available for new data.

## Core Mechanics

- **Block 0**: In the [[custom-dbms]], the very first block of the disk file is reserved for the bitmap.
- **Scanning**: When a relation needs to grow (e.g., creating an [[overflow-blocks]]), the system scans the bitmap for a `0` value.
- **Atomicity**: The bitmap must be updated and "flushed" to disk immediately to ensure that two concurrent operations do not attempt to allocate the same physical block.

## Connections
* **Prerequisites:** Disk block concepts.
* **Used In:** [[custom-dbms]], file systems (e.g., ext4, NTFS).
* **Contrasts With:** Linked-list allocation (where blocks point to the next free block).
