---
tags: [mpi, derived-types, synchronization, acid]
sources: [lec22.txt]
created: 2026-04-20
updated: 2026-04-20
---

# Derived Types and Barriers

**Source:** lec22.txt
**Date ingested:** 2026-04-20
**Type:** Lecture Transcript

## Summary

This lecture covers advanced MPI features including **Derived Datatypes** for bundling different data types into a single communication operation, and **Barriers** for process synchronization. It also introduces Assignment 6, which involves a concurrent transaction processing system, reviewing the **ACID properties** of databases.

## Key Claims

- **Derived Datatypes** reduce communication overhead by allowing multiple data items (even of different types) to be sent in a single message.
- `MPI_Type_create_struct` is the most general function for building derived types.
- `MPI_Barrier` is a synchronization tool that ensures all processes reach a specific point before continuing.
- Performance measurement in MPI is typically done using `MPI_Wtime()`.
- The **ACID properties** (Atomicity, Consistency, Isolation, Durability) are essential for reliable transaction processing.

## Entities Mentioned

- [[mpi]] — Message Passing Interface used for parallel programming.

## Concepts Covered

- [[mpi-derived-datatypes]] — Representing collections of data items in memory.
- [[mpi-barrier]] — Collective synchronization across processes.
- [[performance-measurement-mpi]] — Using `MPI_Wtime` for profiling.
- [[acid-properties]] — Core requirements for database transactions.
