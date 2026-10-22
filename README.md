# FileSearchManager

An Operating Systems course project developed with C++17 on Ubuntu.

The project aims to search text files using multiple threads.

## Current Implementation

The current version lists regular `.txt` files directly inside a selected directory.

- Subdirectories are not searched.
- Only the lowercase `.txt` extension is accepted.
- Invalid directories and incorrect argument counts produce error messages.
- File content searching and multithreaded execution are not implemented yet.

## Requirements

- Ubuntu or Ubuntu on WSL
- A C++17-compatible g++ compiler

## Build

Run the following command from the repository root in Ubuntu:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -pthread -Iinclude src/main.cpp src/file_scanner.cpp src/search.cpp src/parallel_search.cpp -o filesearch
```

## Run the Current Demo

```bash
./filesearch samples
```

Expected output; file order may vary:

```text
samples/a.txt
samples/b.txt
samples/c.txt
Text files found: 3
```

Error examples:

```bash
./filesearch __missing_folder__
./filesearch
```

Exit codes:

- `0`: Directory scanning completed successfully.
- `1`: Directory scanning failed.
- `2`: Incorrect command-line usage.

## Planned Search Command

```bash
./filesearch samples memory 2
```

This command is the target interface and is not supported by the current demo.

The completed program will search for `memory` using two threads. Matching will be case-sensitive and use substring search. Each matching line will be reported once, with its file path and line number.

## Team Responsibilities

| Member | Responsibility | Next deliverable |
| --- | --- | --- |
| Berkay | Command-line interface, directory scanning and output | Validate search arguments and display combined results |
| Zehra | Single-file text searching | Return matching lines, line numbers and file-reading errors |
| Umut | Parallel search execution | Distribute files across workers, merge results safely and join threads |

## Shared Data Structures

Defined in `include/types.hpp`:

- `Match`: File path, line number and matching text.
- `FileListResult`: Discovered file paths and directory scanning errors.
- `SearchResult`: Matches, errors and successfully scanned file count.

## Development

Each member works on a separate branch:

- `feat/berkay-cli-scanner`
- `feat/zehra-file-search`
- `feat/umut-parallel-search`

Compiled executables and build outputs are excluded from Git.