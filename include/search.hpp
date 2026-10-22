#pragma once

#include <string>
#include "types.hpp"

// Tek dosyada arama yapar ve sonuclari dondurur.
SearchResult search_file(
    const std::string& file_path,
    const std::string& query
);
