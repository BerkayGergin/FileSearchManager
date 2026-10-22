
#include "parallel_search.hpp"
#include "search.hpp"

#include <mutex>
#include <thread>
#include <vector>

SearchResult parallel_search(
    const std::vector<std::string>& files,
    const std::string& query,
    std::size_t thread_count
) {
    SearchResult final_result;

    if (query.empty()) {
        final_result.errors.push_back("Search query cannot be empty.");
        return final_result;
    }

    if (thread_count < 1 || thread_count > 8) {
        final_result.errors.push_back("Thread count must be between 1 and 8.");
        return final_result;
    }

    std::mutex result_mutex;
    std::vector<std::thread> threads;

    auto worker = [&](std::size_t id) {
        SearchResult local_result;

        for (std::size_t i = id; i < files.size(); i += thread_count) {
            SearchResult file_result = search_file(files[i], query);

            local_result.matches.insert(
                local_result.matches.end(),
                file_result.matches.begin(),
                file_result.matches.end()
            );

            local_result.errors.insert(
                local_result.errors.end(),
                file_result.errors.begin(),
                file_result.errors.end()
            );

            local_result.files_scanned += file_result.files_scanned;
        }

        std::lock_guard<std::mutex> lock(result_mutex);

        final_result.matches.insert(
            final_result.matches.end(),
            local_result.matches.begin(),
            local_result.matches.end()
        );

        final_result.errors.insert(
            final_result.errors.end(),
            local_result.errors.begin(),
            local_result.errors.end()
        );

        final_result.files_scanned += local_result.files_scanned;
    };

    for (std::size_t id = 0; id < thread_count; ++id) {
        threads.emplace_back(worker, id);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return final_result;
}
