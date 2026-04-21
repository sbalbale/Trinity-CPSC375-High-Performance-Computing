---
aliases: [Relational Operators, Database Operations]
tags: [dbms, mathematics, theory]
sources: [lec17.md]
created: 2026-04-20
updated: 2026-04-20
---

# Relational Algebra

> [!abstract]
> **Relational Algebra** is the mathematical foundation of relational databases. It provides a set of formal operations used to manipulate relations and retrieve data. It is the underlying logic for SQL and the query engine in the [[custom-dbms]].

## Core Operators

### 1. Selection ($\sigma$)
Selects tuples that satisfy a specific condition.
- **Syntax**: $\sigma_{\text{condition}}(\text{Relation})$
- **Example**: $\sigma_{\text{dept\_name} = 'Physics'}(\text{Instructor})$

### 2. Projection ($\pi$)
Selects specific columns (attributes) and removes duplicates.
- **Syntax**: $\pi_{\text{attribute\_list}}(\text{Relation})$
- **Example**: $\pi_{ID, Name}(\text{Instructor})$

### 3. Union ($\cup$)
Combines all tuples from two compatible relations.
- **Requirement**: Compatible schemas (same number of columns and matching domains).

### 4. Set Difference ($-$)
Returns tuples present in the first relation but not the second.

### 5. Natural Join ($\bowtie$)
Combines relations based on matching values in common attributes, removing duplicate columns.
- **Purpose**: Reconstruct complete information spread across multiple tables.

## Query Composition
Operations can be nested to form complex queries.
> [!example] Complex Query
> Find names of CS instructors earning > $90,000:
> $$\pi_{Name}(\sigma_{\text{dept\_name} = 'Computer' \land \text{salary} > 90000}(\text{Instructor}))$$

## Connections
* **Prerequisites:** [[custom-dbms]], Relation concepts.
* **Used In:** Query optimization, physical execution plans.
* **Contrasts With:** SQL (logical language) vs. Relational Algebra (mathematical operations).
