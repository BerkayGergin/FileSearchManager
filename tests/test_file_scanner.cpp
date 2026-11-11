
#include "file_scanner.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

int main() {
    int passed = 0;
    int failed = 0;

    // Test dosyalari icin gecici klasor olustur.
    fs::path root = fs::temp_directory_path() /
        ("filesearch_test_" + std::to_string(
            std::chrono::steady_clock::now()
                .time_since_epoch().count()));

    fs::create_directories(root / "nested");

    auto create_file = [](const fs::path& path,
                          const std::string& content) {
        std::ofstream file(path, std::ios::binary);
        file << content;
    };

    create_file(root / "a.txt", "memory\n");
    create_file(root / "report.csv", "memory,data\n");
    create_file(root / "notes.md", "memory notes\n");
    create_file(root / "events.log", std::string(1500, 'x'));
    create_file(root / "report.json", "{\"memory\":true}\n");
    create_file(root / "nested" / "inner.txt", "memory\n");

    auto check = [&](const std::string& name,
                     bool condition) {
        if (condition) {
            std::cout << "[PASS] " << name << '\n';
            ++passed;
        } else {
            std::cout << "[FAIL] " << name << '\n';
            ++failed;
        }
    };

    // Test 1: Eski .txt davranisi
    auto basic = list_txt_files(root.string());
    check("TXT only",
          basic.errors.empty() && basic.files.size() == 1);

    // Test 2: Birden fazla dosya turu
    FileScanOptions options;
    options.extensions = {
        ".txt", ".csv", ".md", ".log", ".json"
    };

    auto multi = list_files(root.string(), options);
    check("Multiple extensions",
          multi.errors.empty() && multi.files.size() == 5);

    // Test 3: Alt klasorleri de ara
    options.recursive = true;
    auto recursive = list_files(root.string(), options);
    check("Recursive search",
          recursive.errors.empty() && recursive.files.size() == 6);

    // Test 4: Dosya adina gore filtre
    options.filename_contains = "report";
    auto named = list_files(root.string(), options);
    check("Filename filter",
          named.errors.empty() && named.files.size() == 2);

    // Test 5: Belirli dosya secimi
    options = FileScanOptions{};
    options.recursive = true;
    options.selected_relative_paths = {"nested/inner.txt"};
    auto selected = list_files(root.string(), options);
    check("Selected files",
          selected.errors.empty() && selected.files.size() == 1);

    // Test 6: Minimum dosya boyutu
    options = FileScanOptions{};
    options.extensions = {".log"};
    options.min_size_bytes = 1024;
    auto large = list_files(root.string(), options);
    check("File size filter",
          large.errors.empty() && large.files.size() == 1);

    // Test 7: Gecersiz klasor
    auto invalid = list_txt_files(
        (root / "missing").string());
    check("Invalid folder", !invalid.errors.empty());

    // Gecici dosyalari temizle
    std::error_code ec;
    fs::remove_all(root, ec);

    std::cout << "\nPassed: " << passed
              << "\nFailed: " << failed << '\n';

    return failed == 0 ? 0 : 1;
}
