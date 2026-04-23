# Simple Database System

This project implements a multi-threaded database server and a corresponding client. The system features a custom database engine (`mydb`) that supports on-disk storage, a simple relational algebra command set, and a locking mechanism to manage concurrent transactions.

## Project Structure

- **`mydb.c`**: The core database engine. It manages a simulated virtual disk, handles metadata via an in-memory catalog, and implements relational operators like Select, Project, and Natural Join.
- **`server.c`**: A multi-threaded server that wraps the database engine. It uses a worker pool to process client requests, implements a FIFO locking protocol for concurrency control, and manages transaction phases.
- **`client.c`**: A database client that connects to the server via TCP. It parses transaction scripts and routes commands to the server based on assigned client IDs.
- **`Makefile`**: Automates the compilation process for both the server and client.

## Features

### Supported Database Operations

The engine supports the following relational and maintenance commands:

- **`CR`**: Create a new relation with a defined schema.
- **`IN`**: Insert tuples into a base relation.
- **`RM`**: Remove tuples identified by a key.
- **`UP`**: Update existing tuples in-place.
- **`PR`**: Print the schema and all tuples of a relation.
- **`SL`**: Select tuples based on boolean predicates.
- **`PJ`**: Project specific attributes into a new relation.
- **`NJ`**: Perform a natural join between two relations.
- **`UN` / `DF`**: Perform set union and set difference.
- **`DE`**: Delete a relation and free its associated disk blocks.

### Concurrency and Transactions

- **Multi-threading**: The server utilizes a worker pool (`WORKER_COUNT`) and a message queue to handle concurrent requests.
- **Locking**: Implements Shared (Read) and Exclusive (Write) locks at the relation level to ensure data integrity during concurrent access.
- **Deterministic Execution**: Supports phased transactions to ensure clients execute in a predictable order during testing.

## Getting Started

### Compilation

Use the provided `Makefile` to compile the project:

```bash
make
```

This will generate two executables: `server` and `client`.

### Running the System

1.  **Start the Server**:
    In one terminal, start the database server:

    ```bash
    ./server
    ```

2.  **Run the Clients**:
    In another terminal, you can run multiple clients simultaneously. The following command demonstrates running four clients in parallel, feeding them commands from `data.in`, and capturing the output:
    ```bash
    ./client 1 < data.in > c1.out 2>&1 & C1=$!; \
    ./client 2 < data.in > c2.out 2>&1 & C2=$!; \
    ./client 3 < data.in > c3.out 2>&1 & C3=$!; \
    ./client 4 < data.in > c4.out 2>&1 & C4=$!; \
    wait $C1 $C2 $C3 $C4; \
    cat c1.out c2.out c3.out c4.out > myoutput.txt
    ```
