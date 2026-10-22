#pragma once

#include <cstddef>
#include <string>
#include <vector>

struct Match {
    std::string file_path;
    std::size_t line_number;
    std::string line_text;
};

struct FileListResult {
    std::vector<std::string> files;
    std::vector<std::string> errors;
};

struct SearchResult {
    std::vector<Match> matches;
    std::vector<std::string> errors;
    std::size_t files_scanned = 0;
};