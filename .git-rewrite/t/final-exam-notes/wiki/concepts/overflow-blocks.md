---
aliases: [Bucket Chain, Linked Blocks]
tags: [dbms, storage, architecture]
sources: [Homework 17.pdf, lec17.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Overflow Blocks

> [!abstract]
> An **Overflow Block** is a secondary storage block allocated when a primary hash bucket is full. It is linked to the primary block to form a chain, allowing the relation to store more data than a single block can accommodate.

## Core Mechanics

- **Detection**: When a `dbput` operation finds that all slots in the target block are occupied (flag=1).
- **Allocation**: The system requests a new block from the [[bitmap-allocation]] (Block 0).
- **Linking**: The header of the full block is updated with the index of the new overflow block.
- **Search**: During a `dbget`, the system must follow the chain of overflow blocks until the key is found or the end of the chain is reached.

## Connections
* **Prerequisites:** [[dbms-hashing]].
* **Used In:** Maintaining [[base-vs-derived-relations]].
* **Contrasts With:** Linear Probing (another method for handling collisions).
