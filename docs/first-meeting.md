\# FileSearchManager — First Project Meeting



\## Project Overview



FileSearchManager is a C++17 command-line application for Ubuntu.



The completed system will search text files using multiple threads and report matching lines with file paths and line numbers.



Repository: https://github.com/BerkayGergin/FileSearchManager



\## Current Working Version



The current version accepts a directory path and lists its regular `.txt` files.



It does not enter subdirectories. File content searching and parallel execution are not implemented in this version.



The shared skeleton contains:



\- `Match`: File path, line number and matching text.

\- `FileListResult`: Discovered file paths and scanning errors.

\- `SearchResult`: Matches, errors and successfully scanned file count.

\- Separate headers and source files for directory scanning, file searching and parallel execution.



\## Completed Work



\### Berkay



\- Initialized and pushed the shared repository.

\- Added the shared structures and module declarations.

\- Prepared sample text files and build instructions.

\- Implemented directory scanning.

\- Connected directory scanning to the command-line interface.

\- Verified successful scanning, invalid-directory handling and missing-argument handling.

\- Merged the working directory-scanning demo into `main`.



\### Zehra and Umut



Their completed contributions have not yet been confirmed in this meeting record. This section will be updated after each member demonstrates their work.



\## Responsibilities and Next Interim Deliverables



| Member | Owned module | Deliverable by the next interim submission |

| --- | --- | --- |

| Berkay | CLI, directory scanner and output | Validate directory, query and thread arguments; integrate the search modules; display sorted results and errors. |

| Zehra | Single-file search | Read files line by line; return matching lines with 1-based line numbers; handle unreadable, empty and no-match files. |

| Umut | Parallel search | Distribute files across worker threads; call the single-file search function; combine results using a mutex; join all workers. |



The team will verify that one-thread and two-thread searches produce the same matches.



\## Mini Demo



Build from the repository root in Ubuntu:



```bash

g++ -std=c++17 -Wall -Wextra -Wpedantic -pthread -Iinclude src/main.cpp src/file\_scanner.cpp src/search.cpp src/parallel\_search.cpp -o filesearch

```



List the sample files:



```bash

./filesearch samples

```



Expected result: three `.txt` file paths and `Text files found: 3`.



Demonstrate input errors:



```bash

./filesearch \_\_missing\_folder\_\_

./filesearch

```



This PoC demonstrates command-line input, directory traversal and file-path collection.



\## Agreed Search Behaviour



\- Search only `.txt` files directly inside the selected directory.

\- Use case-sensitive substring matching.

\- Report each matching line once.

\- Reject empty queries and invalid thread counts.

\- Count successfully read files, including files with no matches.



\## Team Learning and Collaboration



Each member develops on a separate Git branch and shares small, tested changes through pull requests.



After each completed task, the responsible member adds a short Notion note explaining the change, its verification and any remaining issue.



All three members review the complete program and prepare to explain modules written by their teammates.

