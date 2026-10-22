#include "file_scanner.hpp"
#include "parallel_search.hpp"

#include <algorithm>
#include <charconv>
#include <iostream>
#include <string>
#include <system_error>

int main(int argc, char* argv[]) {
    // 1. Read and validate the arguments.
    if (argc != 4) {
        std::cerr
            << "Usage: filesearch DIRECTORY QUERY THREADS\n"
            << "Example: filesearch samples memory 2\n";
        return 2;
    }

    const std::string folder = argv[1];
    const std::string query = argv[2];
    const std::string thread_text = argv[3];

    if (query.empty()) {
        std::cerr << "Error: Search query cannot be empty.\n";
        return 2;
    }

    unsigned int thread_count = 0;
    const auto conversion = std::from_chars(
        thread_text.data(),
        thread_text.data() + thread_text.size(),
        thread_count
    );

    if (conversion.ec != std::errc{}
        || conversion.ptr != thread_text.data() + thread_text.size()
        || thread_count < 1
        || thread_count > 8) {
        std::cerr
            << "Error: Thread count must be an integer between 1 and 8.\n";
        return 2;
    }

    // 2. Find the files using Berkay's scanner.
    const FileListResult file_list = list_txt_files(folder);

    if (!file_list.errors.empty()) {
        for (const auto& error : file_list.errors) {
            std::cerr << "Error: " << error << '\n';
        }
        return 1;
    }

    // 3. Umut's workers call Zehra's search function.
    SearchResult result = parallel_search(
        file_list.files,
        query,
        thread_count
    );

    // 4. Sort results so thread completion order does not affect display.
    std::sort(
        result.matches.begin(),
        result.matches.end(),
        [](const Match& left, const Match& right) {
            if (left.file_path == right.file_path) {
                return left.line_number < right.line_number;
            }
            return left.file_path < right.file_path;
        }
    );

    // 5. Display matches, errors and the summary.
    for (const auto& match : result.matches) {
        std::cout << match.file_path << ':'
                  << match.line_number << ": "
                  << match.line_text << '\n';
    }

    for (const auto& error : result.errors) {
        std::cerr << "Error: " << error << '\n';
    }

    std::cout << "\nFiles scanned: " << result.files_scanned
              << "\nMatching lines: " << result.matches.size()
              << "\nThreads: " << thread_count << '\n';

    return result.errors.empty() ? 0 : 1;
}
