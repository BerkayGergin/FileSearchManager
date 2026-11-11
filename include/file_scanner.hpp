
#pragma once

#include "types.hpp"

#include <cstdint>
#include <limits>
#include <string>
#include <vector>

// Dosya tarama filtreleri
struct FileScanOptions {

    // Aranacak dosya uzantilari
    std::vector<std::string> extensions = {".txt"};

    // Alt klasorlerde de arama yapilsin mi?
    bool recursive = false;

    // Dosya adina gore filtreleme
    std::string filename_contains;

    // Dosya boyutu filtreleri (byte)
    std::uintmax_t min_size_bytes = 0;

    std::uintmax_t max_size_bytes =
        std::numeric_limits<std::uintmax_t>::max();

    // Kullanici yalnizca belirli dosyalari secebilir
    std::vector<std::string> selected_relative_paths;
};

// Yeni gelismis dosya tarama fonksiyonu
FileListResult list_files(
    const std::string& folder,
    const FileScanOptions& options
);

// Eski surumle uyumluluk
FileListResult list_txt_files(
    const std::string& folder
);
