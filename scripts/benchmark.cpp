
#include "file_scanner.hpp"
#include "parallel_search.hpp"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

using Clock = std::chrono::steady_clock;

void sort_matches(std::vector<Match>& matches) {
    std::sort(matches.begin(), matches.end(),
        [](const Match& a, const Match& b) {
            return std::tie(a.file_path, a.line_number, a.line_text)
                 < std::tie(b.file_path, b.line_number, b.line_text);
        });
}

bool same_matches(
    const std::vector<Match>& a,
    const std::vector<Match>& b
) {
    return a.size() == b.size() &&
        std::equal(a.begin(), a.end(), b.begin(),
            [](const Match& x, const Match& y) {
                return x.file_path == y.file_path &&
                       x.line_number == y.line_number &&
                       x.line_text == y.line_text;
            });
}

int main() {
    const auto files = list_txt_files("benchmark_data");

    if (!files.errors.empty()) {
        std::cerr << "Cannot load benchmark data.\n";
        return 1;
    }

    auto reference = parallel_search(files.files, "memory", 1);
    sort_matches(reference.matches);

    if (!reference.errors.empty() ||
        reference.files_scanned != 100 ||
        reference.matches.size() != 2000) {
        std::cerr << "Reference test failed.\n";
        return 1;
    }

    std::cout << "Files: " << reference.files_scanned
              << " | Matches: " << reference.matches.size()
              << "\n\n";

    for (std::size_t count : {1u, 2u, 4u}) {
        std::vector<double> durations;

        // Warm up before recording measurements.
        parallel_search(files.files, "memory", count);

        for (int repeat = 0; repeat < 5; ++repeat) {
            auto start = Clock::now();

            auto result = parallel_search(
                files.files, "memory", count
            );

            auto finish = Clock::now();

            double elapsed =
                std::chrono::duration<double, std::milli>(
                    finish - start
                ).count();

            sort_matches(result.matches);

            if (!result.errors.empty() ||
                result.files_scanned != reference.files_scanned ||
                !same_matches(result.matches, reference.matches)) {
                std::cerr << "Correctness test failed!\n";
                return 1;
            }

            durations.push_back(elapsed);
        }

        std::sort(durations.begin(), durations.end());

        std::cout << count << " threads | "
                  << "Median: " << std::fixed
                  << std::setprecision(3)
                  << durations[2] << " ms | PASS\n";
    }

    return 0;
}

