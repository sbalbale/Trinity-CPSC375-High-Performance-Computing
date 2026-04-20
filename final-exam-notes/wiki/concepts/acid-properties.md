---
aliases: [ACID, Database Transactions]
tags: [database, transactions, concurrency]
sources: [lec22.txt]
created: 2026-04-20
updated: 2026-04-20
---

# ACID Properties

> [!abstract]
> The **ACID properties** are a set of requirements that guarantee database transactions are processed reliably. In a high-performance concurrent system, ensuring ACID compliance is critical for **data integrity** and preventing race conditions.

## Core Mechanics

- **Atomicity**: Transactions are "all or nothing." If any part of the transaction fails, the entire transaction is rolled back.
- **Consistency**: A transaction must take the database from one valid state to another, maintaining all predefined rules (constraints, cascades, triggers).
- **Isolation**: Concurrent execution of transactions must result in a state identical to if they were executed sequentially.
- **Durability**: Once a transaction is committed, it remains committed even in the event of a system failure (e.g., power outage).

> [!warning] Common Pitfalls
> - **Isolation vs. Performance:** High levels of isolation (e.g., serializable) can lead to significant performance bottlenecks due to locking.
> - **Atomicity Failures:** Without proper logging (Write-Ahead Logging), a crash during a transaction can leave the database in an inconsistent state.

## Implementations & Examples

> [!example] Banking Transaction
> Transferring money from Account A to Account B:
> 1. **Atomicity**: Both the debit from A and credit to B must happen together.
> 2. **Consistency**: The total amount of money across both accounts should remain constant.
> 3. **Isolation**: A simultaneous check of account balances shouldn't see the money "missing" during the transfer.
> 4. **Durability**: Once the "Transaction Complete" message is shown, the balances must be updated on disk.

## Connections
* **Prerequisites:** [[race-condition]], concurrency.
* **Used In:** Concurrent Transaction Processing Systems (Assignment 6).
* **Contrasts With:** BASE (Basically Available, Soft state, Eventual consistency) models used in some NoSQL databases.
