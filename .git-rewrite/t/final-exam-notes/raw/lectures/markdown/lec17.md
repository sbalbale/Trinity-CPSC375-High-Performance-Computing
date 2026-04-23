# Lecture 17: Relational Databases and Transaction Processing Systems

**Course:** CPSC 375 High-Performance Computing  
**Topic:** Database Fundamentals and Concurrent Transaction Processing

---

## Assignment 5: Concurrent Transaction Processing System

### Project Overview
This project is divided into two phases with a focus on building a functional transaction processing system.

### Phase I: Building a Simple Relational Database

**Due:** March 25

**Objectives:**
- Implement core storage engine
- Design and implement schemas
- Implement basic data retrieval mechanisms

**Key Components:**
- Data structure to represent relations (tables)
- Methods for storing and retrieving tuples
- Schema definition and management
- Query execution engine (basic)

### Phase II: Implementing a Concurrent Transaction Processing System

**Due:** April 1

**Objectives:**
- Implement Transaction Processing System (TPS) to handle simultaneous operations
- Maintain ACID compliance during concurrent access
- Implement concurrency control mechanisms
- Handle transaction logging and recovery

**Key Components:**
- Transaction manager
- Concurrency control (locks, timestamps, MVCC)
- Logging and recovery system
- Isolation mechanisms

### Team Structure
- **Individual:** Complete assignment alone
- **Pairs:** Work with one partner
- Either option is acceptable

### Assessment Criteria
- Correctness of implementation
- Efficiency of query execution
- Proper concurrency control
- ACID property maintenance
- Code quality and documentation

---

## Relational Database Fundamentals

### What is a Database?

A **database** is an organized collection of data.

### Database Management System (DBMS)

A **DBMS** manages databases and allows:
- Programs to store data efficiently
- Programs to retrieve data efficiently
- Users to query and modify data
- Multiple concurrent accesses
- Data protection and recovery

### The Relational Model

Most modern databases use the **relational model** for data representation.

**Key Concepts:**
- Data represented as **relations** (tables)
- Structured format with:
  - **Attributes** (columns)
  - **Tuples** (rows)
- Mathematic foundation in set theory
- Simple yet powerful

### Example: Instructor Relation

```
Instructor Table:

┌─────────┬───────────┬─────────────┬──────────┐
│   ID    │   Name    │  Department │  Salary  │
├─────────┼───────────┼─────────────┼──────────┤
│ 12345   │ Adams     │ Computer    │  92,000  │
│ 15946   │ Brown     │ Computer    │  89,000  │
│ 33456   │ Gold      │ Physics     │  87,000  │
│ 98765   │ Green     │ Computer    │  88,000  │
│ 76543   │ Katz      │ Computer    │  93,000  │
└─────────┴───────────┴─────────────┴──────────┘

Attributes (Columns): ID, Name, Department, Salary
Tuples (Rows): 5 instructor records
```

---

## Attributes and Tuples

### Attributes (Columns)

An **attribute** is a column in a relation.

**Domain:** The set of allowed values for each attribute

Examples:
- `ID`: Integer domain (unique employee identifiers)
- `Salary`: Numeric domain (decimal values 0 to 999,999)
- `Name`: String domain (text up to 50 characters)
- `Dept_name`: String domain (department names)

### Tuples (Rows)

A **tuple** is a row in a relation.

**Definition:** An ordered collection of attribute values

Example tuple: `(33456, Gold, Physics, 87000)`

### Key Properties

1. **Relation Schema**
   - Defined structure of the relation
   - Column names and their domains
   - Example: `Instructor(ID:integer, Name:string, Dept_name:string, Salary:numeric)`

2. **Relation Instance**
   - Actual data contained in the relation at a given time
   - Set of tuples (rows)
   - Changes as data is inserted, updated, or deleted

---

## Relations Are Unordered

### Important Principle

The **order of tuples is irrelevant** in relational databases.

- Tuples may be stored in arbitrary order
- Two relations with same tuples in different order are considered identical
- Query results may appear in any order (unless ORDER BY specified)

### Implication for Implementation

```
Version 1 (Table A):
┌─────────┬───────────┬─────────────┬──────────┐
│   ID    │   Name    │  Department │  Salary  │
├─────────┼───────────┼─────────────┼──────────┤
│ 12345   │ Adams     │ Computer    │  92,000  │
│ 33456   │ Gold      │ Physics     │  87,000  │
│ 76543   │ Katz      │ Computer    │  93,000  │
└─────────┴───────────┴─────────────┴──────────┘

Version 2 (Table B) - Same data, different order:
┌─────────┬───────────┬─────────────┬──────────┐
│   ID    │   Name    │  Department │  Salary  │
├─────────┼───────────┼─────────────┼──────────┤
│ 76543   │ Katz      │ Computer    │  93,000  │
│ 12345   │ Adams     │ Computer    │  92,000  │
│ 33456   │ Gold      │ Physics     │  87,000  │
└─────────┴───────────┴─────────────┴──────────┘

Tables A and B are logically equivalent!
```

### Consequences
- Indexes used for performance (physical organization)
- Logical queries produce consistent results regardless of physical storage order
- Optimization can reorder tuples internally

---

## Keys

### Definition

A **primary key** is one or more attributes that uniquely identify a tuple.

### Characteristics

1. **Uniqueness**
   - No two tuples have the same primary key value
   - Cannot be NULL
   - Enforced by DBMS

2. **Minimality**
   - No proper subset of key attributes is also a key
   - Reduces storage and improves performance

3. **Non-null**
   - Every tuple must have a value for the primary key
   - NULL not allowed for key attributes

### Example: Instructor Relation

```
┌─────────┬───────────┬─────────────┬──────────┐
│   ID    │   Name    │  Department │  Salary  │
├─────────┼───────────┼─────────────┼──────────┤
│ 12345   │ Adams     │ Computer    │  92,000  │ ← ID is unique
│ 15946   │ Brown     │ Computer    │  89,000  │
│ 33456   │ Gold      │ Physics     │  87,000  │
│ 98765   │ Green     │ Computer    │  88,000  │
│ 76543   │ Katz      │ Computer    │  93,000  │
└─────────┴───────────┴─────────────┴──────────┘

Primary Key: ID
```

### Other Types of Keys

**Candidate Keys:** Attributes that could serve as primary key
- Must satisfy uniqueness property
- DBMS recognizes multiple candidate keys

**Foreign Keys:** Attributes that reference primary key in another table
- Create relationships between tables
- Enable normalization and referential integrity

---

## Relational Algebra

### Overview

Relational algebra provides **operations on relations** and forms the mathematical foundation of relational databases.

### Five Core Operators

1. **Selection** (σ - sigma)
2. **Projection** (π - pi)
3. **Union** (∪)
4. **Difference** (−)
5. **Natural Join** (⋈)

### Additional Operations

- **Cartesian Product** (×)
- **Intersection** (∩)
- **Rename** (ρ)
- **Assignment** (←)

---

## Selection (σ)

### Definition

Selects tuples that satisfy a given condition.

### Syntax

```
σ(condition)(relation)
```

### Example

**Task:** Select instructors in the "Physics" department

```
Result = σ(dept_name = 'Physics')(Instructor)
```

**Input (Instructor):**
```
┌─────────┬───────────┬─────────────┬──────────┐
│   ID    │   Name    │  Department │  Salary  │
├─────────┼───────────┼─────────────┼──────────┤
│ 12345   │ Adams     │ Computer    │  92,000  │
│ 33456   │ Gold      │ Physics     │  87,000  │ ← Selected
│ 76543   │ Katz      │ Computer    │  93,000  │
└─────────┴───────────┴─────────────┴──────────┘

Result:
┌─────────┬───────────┬─────────────┬──────────┐
│   ID    │   Name    │  Department │  Salary  │
├─────────┼───────────┼─────────────┼──────────┤
│ 33456   │ Gold      │ Physics     │  87,000  │
└─────────┴───────────┴─────────────┴──────────┘
```

### Conditions

Can use comparison operators:
- `=`, `≠`, `<`, `≤`, `>`, `≥`
- Logical operators: AND, OR, NOT
- Example: `salary > 90000 AND dept_name = 'Computer'`

---

## Projection (π)

### Definition

Selects specific columns (attributes) from a relation.

### Syntax

```
π(attribute_list)(relation)
```

### Example

**Task:** Show ID, Name, and Salary (eliminate dept_name)

```
Result = π(ID, Name, Salary)(Instructor)
```

**Input (Instructor):**
```
┌─────────┬───────────┬─────────────┬──────────┐
│   ID    │   Name    │  Department │  Salary  │
├─────────┼───────────┼─────────────┼──────────┤
│ 12345   │ Adams     │ Computer    │  92,000  │
│ 33456   │ Gold      │ Physics     │  87,000  │
│ 76543   │ Katz      │ Computer    │  93,000  │
└─────────┴───────────┴─────────────┴──────────┘

Result (order is irrelevant):
┌─────────┬───────────┬──────────┐
│   ID    │   Name    │  Salary  │
├─────────┼───────────┼──────────┤
│ 12345   │ Adams     │  92,000  │
│ 33456   │ Gold      │  87,000  │
│ 76543   │ Katz      │  93,000  │
└─────────┴───────────┴──────────┘
```

### Important Note
- Duplicates are automatically removed in projection
- If multiple tuples project to same row, only one appears
- Cardinality of result ≤ cardinality of input

---

## Union (∪)

### Definition

Combines tuples from two relations.

### Syntax

```
Relation1 ∪ Relation2
```

### Example

**Scenario:** Course sections in Fall 2017 and Spring 2018

**Fall 2017 Sections:**
```
┌──────────┬─────────────┐
│ Course   │ Section     │
├──────────┼─────────────┤
│ CS101    │ Fall 2017   │
│ CS201    │ Fall 2017   │
│ MATH101  │ Fall 2017   │
└──────────┴─────────────┘

Spring 2018 Sections:
┌──────────┬─────────────┐
│ Course   │ Section     │
├──────────┼─────────────┤
│ CS101    │ Spring 2018 │
│ PHYS101  │ Spring 2018 │
│ CS201    │ Spring 2018 │
└──────────┴─────────────┘

Union Result:
┌──────────┬─────────────┐
│ Course   │ Section     │
├──────────┼─────────────┤
│ CS101    │ Fall 2017   │
│ CS201    │ Fall 2017   │
│ MATH101  │ Fall 2017   │
│ CS101    │ Spring 2018 │
│ PHYS101  │ Spring 2018 │
│ CS201    │ Spring 2018 │
└──────────┴─────────────┘
```

### Requirements
- Relations must have **compatible schemas**
- Same number of columns
- Corresponding attributes have compatible domains

---

## Difference (−)

### Definition

Returns tuples in one relation but not the other.

### Syntax

```
Relation1 − Relation2
```

### Example

**Task:** Find courses taught in Fall 2017 but not in Spring 2018

**Fall 2017:**
```
CS101, CS201, MATH101
```

**Spring 2018:**
```
CS101, PHYS101, CS201
```

**Result (Fall 2017 − Spring 2018):**
```
MATH101
```

The tuple for MATH101 appears in Fall 2017 but not in Spring 2018.

### Properties
- Order matters: A − B ≠ B − A
- Only contains tuples unique to first relation
- Relations must have compatible schemas

---

## Natural Join (⋈)

### Definition

Combines relations based on matching attributes.

### Purpose
- Connect information spread across multiple relations
- Enforce relationships between entities
- Reconstruct complete information

### Example

**Instructor Table:**
```
┌─────────┬───────────┬─────────────┐
│   ID    │   Name    │  Department │
├─────────┼───────────┼─────────────┤
│ 12345   │ Adams     │ Computer    │
│ 33456   │ Gold      │ Physics     │
└─────────┴───────────┴─────────────┘
```

**Department Table:**
```
┌─────────────┬──────────────┐
│ Department  │ Office Building│
├─────────────┼──────────────┤
│ Computer    │ Tech Hall    │
│ Physics     │ Science Bldg │
└─────────────┴──────────────┘
```

**Natural Join Result:**
```
Instructor ⋈ Department

┌─────────┬───────────┬─────────────┬──────────────┐
│   ID    │   Name    │  Department │ Office Building│
├─────────┼───────────┼─────────────┼──────────────┤
│ 12345   │ Adams     │ Computer    │ Tech Hall    │
│ 33456   │ Gold      │ Physics     │ Science Bldg │
└─────────┴───────────┴─────────────┴──────────────┘
```

The join happens on the common attribute "Department"

### Key Points
- Performed on common attributes
- Eliminates duplicate columns
- Returns only matching tuples
- Order of operands doesn't matter (mathematically commutative)

---

## Combining Operations

### Query Composition

Relational algebra operations can be composed:

**Example:** Find names of instructors in Computer Science earning more than $90,000

```
π(Name)(σ(dept_name = 'Computer' AND salary > 90000)(Instructor))
```

**Execution:**
1. Select tuples where dept = 'Computer' AND salary > 90000
2. Project only the Name attribute
3. Return results

---

## Summary

### Relational Model Principles

1. **Data Organization**
   - Relations (tables) with attributes and tuples
   - Simple, intuitive structure
   - Mathematical foundation

2. **Uniqueness**
   - Primary keys ensure tuple uniqueness
   - Foreign keys establish relationships

3. **Unordered Nature**
   - Tuple order irrelevant
   - Enables flexible implementation
   - Logical vs. physical organization

4. **Query Foundation**
   - Relational algebra provides formal query language
   - Operations compose for complex queries
   - Foundation for SQL

### Connection to This Course

- Understanding relational model essential for database implementation
- Concurrency issues arise when multiple transactions access same data
- ACID properties ensure consistency despite concurrent modifications
- Next phase: Implementing transactions with proper locking and isolation

---

## References

- Silberschatz, A., Korth, H. F., & Sudarshan, S. (2020). Database System Concepts (7th ed.)
- Course Website: https://www.cs.trincoll.edu/~pyoon/cpsc375/
