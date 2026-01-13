# In-Memory File System (FSS) CLI version.

## Overview

**FSS File System Simulator (CLI version)** is a Unix-like, in-memory file system written in **C**, designed to deeply understand how real file systems work internally. Instead of relying on the host OS, FSS maintains its own directory tree, file objects, and path resolution logic entirely in user space.

This project focuses on **systems-level concepts** such as:
- Path resolution (`/a/b/c`, quoted paths, nested traversal)
- Directory and file management
- Memory management and data structures
- Separation of concerns between *shell*, *filesystem core*, and *path handling*

The goal is not just functionality, but **learning and demonstrating low-level design skills** that are highly relevant for backend, systems, and infrastructure roles.

---

## Key Features

### ✅ In-Memory File System Core
- Custom `FS` structure representing the file system
- Independent from OS file system calls
- Tree-based directory hierarchy

### ✅ Path Resolution (Core Feature)
- Absolute and relative paths
- Nested directory traversal (`/doc/"imp doc"/"very imp doc"`)
- Centralized path parsing using `path.c` and `path.h`
- Shell handles quotes, FS core works on resolved tokens

### ✅ Directory Management
- Create directories at any depth
- Validate path existence before creation
- Prevent duplicate directory creation

### ✅ File Management
- Create files using full paths
- Detect duplicate files
- Store file content in memory

### ✅ Separation of Responsibilities
- **Shell (`main.c`)**: command parsing, quotes handling, user interaction
- **Filesystem Core (`FS.c`)**: directory/file logic
- **Path Module (`path.c`)**: path tokenization and resolution

---

## Project Structure

```
.
├── FS.c / FS.h          # File system core logic
├── path.c / path.h      # Path parsing and resolution
├── shell / main.c       # CLI shell (command handling)
├── data_structures/     # Hashmaps, stacks, trees
├── README.md
```

---

## Example Path Handling

```sh
cd /doc/"imp doc"/"very imp doc"
touch /doc/projects/demo.txt
mkdir /a/b/c
```

- Quoted paths are handled by the shell
- FS core operates on clean, tokenized paths

---

## Why This Project Matters

Unlike typical CRUD or web-only projects, this project demonstrates:

- Understanding of **file system internals**
- Practical use of **pointers, structs, and dynamic memory**
- Ability to design **modular C systems**
- Debugging and reasoning about complex state

This project is intentionally built "from scratch" without external libraries to strengthen core fundamentals.

---

## Planned Features (Roadmap)

### 🔜 Persistence (High Priority)
- Save file system state to disk
- Reload directory tree on startup
- Consistent on-disk format (binary or JSON snapshot)
- copying files and folders form one directory to another.

---

## Technologies Used

- **C** (core filesystem and data structures)

---

## Learning Outcomes

Through this project, I learned:
- How path resolution works internally
- How shells differ from file system cores
- Designing rollback-safe directory traversal
- Memory safety and error handling in C
- Structuring a large C codebase

---

## Status

🚧 **Actively under development**

Current focus:
- Finalizing path resolution
- Fixing edge-case bugs
- Implementing persistence

---

## Author

**Nilabha Samadder**

> Built to strengthen systems programming fundamentals and prepare for backend / infrastructure roles.

