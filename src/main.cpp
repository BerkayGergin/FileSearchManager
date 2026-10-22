#include "file_scanner.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: filesearch DIRECTORY\n";
        return 2;
    }

    const FileListResult result = list_txt_files(argv[1]);

    if (!result.errors.empty()) {
        for (const auto& error : result.errors) {
            std::cerr << "Error: " << error << '\n';
        }
        return 1;
    }

    for (const auto& file : result.files) {
        std::cout << file << '\n';
    }

    std::cout << "Text files found: "
              << result.files.size() << '\n';

    return 0;
}
