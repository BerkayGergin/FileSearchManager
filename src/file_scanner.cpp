#include "file_scanner.hpp"

#include <filesystem>

FileListResult list_txt_files(const std::string& folder) {
    FileListResult result;
    namespace fs = std::filesystem;

    try {
        if (!fs::is_directory(folder)) {
            result.errors.push_back(
                "Not a valid directory: " + folder
            );
            return result;
        }

        for (const auto& entry : fs::directory_iterator(folder)) {
            if (entry.is_regular_file()
                && entry.path().extension() == ".txt") {
                result.files.push_back(entry.path().string());
            }
        }
    } catch (const fs::filesystem_error& error) {
        result.files.clear();
        result.errors.push_back(
            std::string("Directory scan failed: ") + error.what()
        );
    }

    return result;
}
