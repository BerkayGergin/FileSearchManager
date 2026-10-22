\# FileSearchManager



A C++17 command-line project for searching text files using multiple threads.



\## Current Status



The shared data structures, function declarations, sample files and source skeleton are available.



The current executable only prints a skeleton message. Directory scanning, text searching and parallel execution are not implemented yet.



\## Requirements



\- Ubuntu or Ubuntu on WSL

\- A C++17-compatible g++ compiler



\## Build



Run from the repository root in Ubuntu:



```bash

g++ -std=c++17 -Wall -Wextra -Wpedantic -pthread -Iinclude src/main.cpp src/file\_scanner.cpp src/search.cpp src/parallel\_search.cpp -o filesearch

```



\## Run the Current Skeleton



```bash

./filesearch

```



\## Planned Usage



After implementation:



```bash

./filesearch samples memory 2

```



This will search the `.txt` files directly inside `samples` using two threads.



Search will be case-sensitive and use substring matching. Each matching line will be reported once. Subdirectories will not be searched.



\## Team Responsibilities



| Member | Module |

| --- | --- |

| Berkay | Command-line input, directory scanning and result display |

| Zehra | Searching one file and reporting matching lines |

| Umut | Distributing files across threads and combining results |



\## Shared Interfaces



\- `include/types.hpp`

\- `include/file\_scanner.hpp`

\- `include/search.hpp`

\- `include/parallel\_search.hpp`



Compiled executables and build outputs are excluded from Git.

