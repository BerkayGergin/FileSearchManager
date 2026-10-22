# FileSearchManager - First Project Meeting

## Repository

https://github.com/BerkayGergin/FileSearchManager

## Project Goal

A C++17 application for Ubuntu that searches text files using multiple threads and reports matching lines with file paths and line numbers.

## Completed Contributions

| Member | Completed contribution |
| --- | --- |
| Berkay | Shared repository and skeleton, directory scanner, CLI validation, module integration, sorted output and documentation |
| Zehra | Single-file search, line numbering, substring matching, read-error handling, sample cases and a demonstration test program |
| Umut | Parallel search, file distribution, thread-local results, mutex-protected merging and joining workers |

All three modules are integrated into the command-line application.

## Core Structures

Defined in include/types.hpp:

- Match: file path, line number and matching text.
- FileListResult: discovered file paths and directory scanning errors.
- SearchResult: matching lines, errors and successfully scanned file count.

Each module has a separate header and source file.

## Build

Run from the repository root in Ubuntu:

~~~bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -pthread -Iinclude src/main.cpp src/file_scanner.cpp src/search.cpp src/parallel_search.cpp -o filesearch
~~~

## Working Demo

~~~bash
./filesearch samples memory 1
./filesearch samples memory 2
./filesearch samples memory 2x
~~~

Observed results:

- Both valid runs returned the same six matching lines.
- Seven files were successfully scanned.
- The invalid thread argument was rejected.

The demo demonstrates directory traversal, file reading, worker threads, synchronized result merging and input validation.

## Search Rules

- Search regular .txt files directly inside the selected directory.
- Do not enter subdirectories.
- Use case-sensitive substring matching.
- Report each matching line once.
- Sort output by file path and line number.
- Accept between 1 and 8 threads.

## Next Interim Deliverables

| Member | Next deliverable |
| --- | --- |
| Berkay | Expand CLI and integration checks, document error cases and maintain demo instructions |
| Zehra | Add automatic result checks and expand query and file-reading test cases |
| Umut | Compare correctness and elapsed time with 1, 2 and 4 threads on a larger dataset; handle thread-start failures safely |

Performance will be measured without assuming that more threads are always faster.

## Collaboration and Learning

Members work on individual branches and combine changes through pull requests.

After completed tasks, members share short Notion notes describing the change, verification and remaining work.

Every member reviews all modules and prepares to explain the complete program.

The recorded sample checks are initial verification, not exhaustive testing.
