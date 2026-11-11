
#include "file_scanner.hpp"

#include <algorithm>
#include <filesystem>
#include <string>
#include <system_error>

namespace fs = std::filesystem;

FileListResult list_files(
    const std::string& folder,
    const FileScanOptions& options
) {
    FileListResult result;

    try {
        fs::path root(folder);

        // Klasor kontrolu
        if (!fs::is_directory(root)) {
            result.errors.push_back(
                "Invalid directory: " + folder
            );
            return result;
        }

        if (options.min_size_bytes > options.max_size_bytes) {
            result.errors.push_back(
                "Minimum file size exceeds maximum."
            );
            return result;
        }

        // Her dosya icin filtreleri uygula
        auto check_file = [&](const fs::directory_entry& entry) {

            if (!entry.is_regular_file()) {
                return;
            }

            fs::path path = entry.path();

            // Uzanti filtresi
            std::string extension = path.extension().string();

            if (std::find(
                    options.extensions.begin(),
                    options.extensions.end(),
                    extension
                ) == options.extensions.end()) {
                return;
            }

            // Dosya adi filtresi
            if (!options.filename_contains.empty()) {
                std::string filename = path.filename().string();

                if (filename.find(
                        options.filename_contains
                    ) == std::string::npos) {
                    return;
                }
            }

            // Dosya boyutu filtresi
            std::error_code ec;
            auto size = entry.file_size(ec);

            if (ec) {
                result.errors.push_back(
                    "Cannot read file size: " + path.string()
                );
                return;
            }

            if (size < options.min_size_bytes ||
                size > options.max_size_bytes) {
                return;
            }

            // Belirli dosyalari secme
            if (!options.selected_relative_paths.empty()) {

                std::string relative =
                    path.lexically_relative(root).generic_string();

                if (std::find(
                        options.selected_relative_paths.begin(),
                        options.selected_relative_paths.end(),
                        relative
                    ) == options.selected_relative_paths.end()) {
                    return;
                }
            }

            result.files.push_back(path.string());
        };

        // Alt klasorler dahil tarama
        if (options.recursive) {
            for (const auto& entry :
                 fs::recursive_directory_iterator(
                     root,
                     fs::directory_options::skip_permission_denied
                 )) {
                check_file(entry);
            }
        }
        else {
            // Sadece ana klasoru tara
            for (const auto& entry :
                 fs::directory_iterator(
                     root,
                     fs::directory_options::skip_permission_denied
                 )) {
                check_file(entry);
            }
        }

        // Dosyalari sirala
        std::sort(
            result.files.begin(),
            result.files.end()
        );

    }
    catch (const fs::filesystem_error& error) {
        result.errors.push_back(error.what());
    }

    return result;
}

// Eski surumle uyumluluk
FileListResult list_txt_files(
    const std::string& folder
) {
    FileScanOptions options;
    return list_files(folder, options);
}
