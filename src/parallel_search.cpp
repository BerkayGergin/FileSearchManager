
#include "parallel_search.hpp"
#include "search.hpp"

#include <exception>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

SearchResult parallel_search(
    const std::vector<std::string>& files,
    const std::string& query,
    std::size_t thread_count
) {
    SearchResult final_result;

    // Validate inputs.
    if (query.empty()) {
        final_result.errors.push_back(
            "Search query cannot be empty."
        );
        return final_result;
    }

    if (thread_count < 1 || thread_count > 8) {
        final_result.errors.push_back(
            "Thread count must be between 1 and 8."
        );
        return final_result;
    }

    std::mutex result_mutex;
    std::vector<std::thread> threads;

    auto worker = [&](std::size_t id) {
        SearchResult local_result;

        try {
            for (std::size_t i = id;
                 i < files.size();
                 i += thread_count) {

                SearchResult file_result =
                    search_file(files[i], query);

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

                local_result.files_scanned +=
                    file_result.files_scanned;
            }
        }
        catch (const std::exception& error) {
            local_result.errors.push_back(
                std::string("Worker error: ") +
                error.what()
            );
        }

        // Protect only the shared result merge.
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

        final_result.files_scanned +=
            local_result.files_scanned;
    };

    std::string launch_error;

    // Create worker threads safely.
    try {
        threads.reserve(thread_count);

        for (std::size_t id = 0;
             id < thread_count;
             ++id) {

            threads.emplace_back(worker, id);
        }
    }
    catch (const std::exception& error) {
        launch_error =
            std::string("Thread start failed: ") +
            error.what();
    }

    // Wait for every successfully started thread.
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    if (!launch_error.empty()) {
        final_result.errors.push_back(launch_error);
    }


    return final_result;
}

