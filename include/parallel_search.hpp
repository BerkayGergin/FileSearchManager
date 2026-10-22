
#pragma once

#include "types.hpp"

#include <cstddef>
#include <string>
#include <vector>

SearchResult parallel_search(
    const std::vector<std::string>& files,
    const std::string& query,
    std::size_t thread_count
);


