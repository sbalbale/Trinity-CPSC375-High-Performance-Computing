---
aliases: [Data Dictionary, Catalog and Columns]
tags: [dbms, storage, metadata]
sources: [Homework 17.pdf, lec17.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Relation Metadata

> [!abstract]
> **Relation Metadata** refers to the descriptive information about the database schema (tables and columns). In the [[custom-dbms]], this is stored in two system tables: the `catalog` and the `columns`.

## System Tables

### 1. Catalog
Stores top-level table properties:
- `Relname`: Unique name of the relation.
- `Kind`: Base (`B`) or Derived (`D`).
- `Attsize`: Number of attributes.
- `Keysize`: Number of attributes in the primary key.
- `Relsize`: Current number of tuples.

### 2. Columns
Stores attribute-level details:
- `Relname`: Table the column belongs to.
- `Attname`: Name of the attribute.
- `Attdomain`: Data type (e.g., `S` for String, `I` for Integer).
- `Attposition`: Logical order in the tuple.

## Connections
* **Prerequisites:** [[custom-dbms]].
* **Used In:** Schema verification during SQL-like commands (`CR`, `DE`, `SL`).
* **Contrasts With:** Hardcoded schemas (not used in relational systems).
