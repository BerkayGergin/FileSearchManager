# FileSearchManager

A multithreaded text file search application for an Operating Systems course.

Built with C++17 for Ubuntu.

## Requirements

- Ubuntu or Ubuntu on WSL
- A C++17-compatible g++ compiler

## Build

Run from the repository root in Ubuntu:

~~~bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -pthread -Iinclude src/main.cpp src/file_scanner.cpp src/search.cpp src/parallel_search.cpp -o filesearch
~~~

## Run

~~~bash
./filesearch DIRECTORY QUERY THREADS
./filesearch samples memory 2
./filesearch samples "shared memory" 2
~~~

THREADS must be an integer between 1 and 8.

The current sample directory contains seven text files. Searching for `memory` produces six matching lines.

Example match:

~~~text
samples/a.txt:2: memory management
~~~

Expected summary for the two-thread example:

~~~text
Files scanned: 7
Matching lines: 6
Threads: 2
~~~

## Search Rules

- Search regular files with the lowercase `.txt` extension.
- Search only the selected directory; do not enter subdirectories.
- Use case-sensitive substring matching.
- Report each matching line once.
- Number lines starting from 1.
- Sort matches by file path and line number.
- Count successfully read files, including empty files and files without matches.
- Reject empty queries, invalid directories and invalid thread counts.

## Error Examples

~~~bash
./filesearch __missing_folder__ memory 2
./filesearch samples memory 0
./filesearch samples memory 2x
~~~

## Exit Codes

- `0`: Search completed without reported errors, including zero matches.
- `1`: Directory scanning or file searching reported an error.
- `2`: Invalid command-line arguments.

## Team Responsibilities

| Member | Responsibility |
| --- | --- |
| Berkay | CLI validation, directory scanning, integration and sorted output |
| Zehra | Reading one file and returning matching lines and errors |
| Umut | Worker threads, file distribution and synchronized result merging |

Shared structures and function declarations are in `include/`.

Workers collect results locally. A mutex protects merging into the shared result. All workers are joined before returning the final result.

## Verification

The integrated program was manually checked with one and two threads. Both runs returned the same six matching lines from seven sample files.

The invalid thread argument `2x` was rejected.

Single-file demonstration output is available in `docs/zehra-test-output.txt`.

These checks do not establish a performance improvement.

## Collaboration

Use individual feature branches and pull requests.

After completed tasks, share short Notion notes explaining the change, verification and remaining work.

Compiled executables and build outputs are excluded from Git.
