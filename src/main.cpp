
#include "file_scanner.hpp"
#include "parallel_search.hpp"

#include <algorithm>
#include <charconv>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <system_error>
#include <vector>

// Sayinin gecerli olup olmadigini kontrol eder
bool parse_number(const std::string& text,
                  std::uint64_t& number) {
    auto result = std::from_chars(
        text.data(),
        text.data() + text.size(),
        number
    );

    return result.ec == std::errc{} &&
           result.ptr == text.data() + text.size();
}

// JSON icindeki ozel karakterleri guvenli yazar
void print_json_string(const std::string& text) {
    const char* hex = "0123456789abcdef";

    std::cout << '"';

    for (unsigned char ch : text) {
        switch (ch) {
            case '"':
                std::cout << "\\\"";
                break;
            case '\\':
                std::cout << "\\\\";
                break;
            case '\n':
                std::cout << "\\n";
                break;
            case '\r':
                std::cout << "\\r";
                break;
            case '\t':
                std::cout << "\\t";
                break;
            default:
                if (ch < 32) {
                    std::cout << "\\u00"
                              << hex[ch >> 4]
                              << hex[ch & 15];
                } else {
                    std::cout << static_cast<char>(ch);
                }
        }
    }

    std::cout << '"';
}

// JSON cikti fonksiyonu
void print_json_result(
    const SearchResult& result,
    std::size_t discovered,
    std::uint64_t threads,
    double scan_ms,
    double search_ms,
    double total_ms
) {
    std::cout << std::fixed << std::setprecision(3);

    std::cout << "{\n";

    std::cout << "\"files_discovered\":"
              << discovered << ",\n";

    std::cout << "\"files_scanned\":"
              << result.files_scanned << ",\n";

    std::cout << "\"matching_lines\":"
              << result.matches.size() << ",\n";

    std::cout << "\"threads\":"
              << threads << ",\n";

    std::cout << "\"scan_ms\":"
              << scan_ms << ",\n";

    std::cout << "\"search_ms\":"
              << search_ms << ",\n";

    std::cout << "\"total_ms\":"
              << total_ms << ",\n";

    std::cout << "\"matches\": [\n";

    for (std::size_t i = 0;
         i < result.matches.size(); ++i) {

        const auto& match = result.matches[i];

        std::cout << "{ \"file_path\":";
        print_json_string(match.file_path);

        std::cout << ", \"line_number\":"
                  << match.line_number;

        std::cout << ", \"line_text\":";
        print_json_string(match.line_text);

        std::cout << "}";

        if (i + 1 < result.matches.size()) {
            std::cout << ",";
        }

        std::cout << "\n";
    }

    std::cout << "],\n\"errors\": [";

    for (std::size_t i = 0;
         i < result.errors.size(); ++i) {

        if (i > 0) std::cout << ",";

        print_json_string(result.errors[i]);
    }

    std::cout << "]\n}\n";
}

int main(int argc, char* argv[]) {

    if (argc < 4) {
        std::cerr
            << "Usage: filesearch DIRECTORY QUERY THREADS [OPTIONS]\n"
            << "Options:\n"
            << "  --extensions .txt,.csv,.md\n"
            << "  --recursive\n"
            << "  --filename TEXT\n"
            << "  --min-kb NUMBER\n"
            << "  --max-kb NUMBER\n"
            << "  --file RELATIVE_PATH\n"
            << "  --json\n"
            << "  --quiet\n";

        return 2;
    }

    const std::string folder = argv[1];
    const std::string query = argv[2];

    // Arama ifadesini kontrol et
    if (query.empty()) {
        std::cerr << "Error: Empty search query.\n";
        return 2;
    }

    // Thread sayisini kontrol et
    std::uint64_t thread_count = 0;

    if (!parse_number(argv[3], thread_count) ||
        thread_count < 1 || thread_count > 8) {

        std::cerr << "Error: Invalid thread count.\n";
        return 2;
    }

    FileScanOptions options;

    bool json_output = false;
    bool quiet_output = false;

    // Komut satiri filtreleri
    for (int i = 4; i < argc; ++i) {

        std::string arg = argv[i];

        if (arg == "--recursive") {
            options.recursive = true;
        }

        else if (arg == "--json") {
            json_output = true;
        }

        else if (arg == "--quiet") {
            quiet_output = true;
        }

        else if (arg == "--extensions") {

            if (++i >= argc) {
                std::cerr << "Error: Missing extensions.\n";
                return 2;
            }

            std::string value = argv[i];

            if (value.empty() || value.back() == ',') {
                std::cerr << "Error: Invalid extensions.\n";
                return 2;
            }

            std::stringstream stream(value);
            std::string extension;
            std::vector<std::string> extensions;

            while (std::getline(stream, extension, ',')) {

                if (extension.size() < 2 ||
                    extension.front() != '.') {

                    std::cerr << "Error: Invalid extension.\n";
                    return 2;
                }

                extensions.push_back(extension);
            }

            options.extensions = extensions;
        }

        else if (arg == "--filename") {

            if (++i >= argc) {
                std::cerr << "Error: Missing filename.\n";
                return 2;
            }

            options.filename_contains = argv[i];
        }

        else if (arg == "--file") {

            if (++i >= argc || std::string(argv[i]).empty()) {
                std::cerr << "Error: Missing file path.\n";
                return 2;
            }

            options.selected_relative_paths.push_back(argv[i]);
        }

        else if (arg == "--min-kb" ||
                 arg == "--max-kb") {

            if (++i >= argc) {
                std::cerr << "Error: Missing file size.\n";
                return 2;
            }

            std::uint64_t size = 0;

            if (!parse_number(argv[i], size) ||
                size > std::numeric_limits<
                    std::uintmax_t>::max() / 1024) {

                std::cerr << "Error: Invalid file size.\n";
                return 2;
            }

            if (arg == "--min-kb") {
                options.min_size_bytes = size * 1024;
            } else {
                options.max_size_bytes = size * 1024;
            }
        }

        else {
            std::cerr << "Error: Unknown option: "
                      << arg << '\n';
            return 2;
        }
    }

    if (options.min_size_bytes >
        options.max_size_bytes) {

        std::cerr << "Error: Invalid size range.\n";
        return 2;
    }

    using Clock = std::chrono::steady_clock;

    auto start = Clock::now();

    // BERKAY: Dosya tarama
    FileListResult file_list = list_files(
        folder, options
    );

    auto after_scan = Clock::now();

    SearchResult result;

    if (!file_list.errors.empty()) {
        result.errors = file_list.errors;
    } else {

        // UMUT: Paralel arama
        // ZEHRA: Tek dosyada arama
        result = parallel_search(
            file_list.files,
            query,
            static_cast<std::size_t>(thread_count)
        );
    }

    auto after_search = Clock::now();

    // Sonuclari dosya ve satir numarasina gore sirala
    std::sort(
        result.matches.begin(),
        result.matches.end(),
        [](const Match& a, const Match& b) {
            if (a.file_path != b.file_path)
                return a.file_path < b.file_path;

            return a.line_number < b.line_number;
        }
    );

    auto after_sort = Clock::now();

    auto to_ms = [](auto first, auto second) {
        return std::chrono::duration<double, std::milli>(
            second - first
        ).count();
    };

    double scan_ms = to_ms(start, after_scan);
    double search_ms = to_ms(after_scan, after_search);
    double total_ms = to_ms(start, after_sort);

    // Arayuz icin JSON cikti
    if (json_output) {

        print_json_result(
            result,
            file_list.files.size(),
            thread_count,
            scan_ms,
            search_ms,
            total_ms
        );
    }

    // Normal terminal ciktisi
    else {

        if (!quiet_output) {
            for (const auto& match : result.matches) {

                std::cout << match.file_path << ":"
                          << match.line_number << ": "
                          << match.line_text << '\n';
            }
        }

        for (const auto& error : result.errors) {
            std::cerr << "Error: " << error << '\n';
        }

        std::cout << "\nFiles scanned: "
                  << result.files_scanned
                  << "\nMatching lines: "
                  << result.matches.size()
                  << "\nThreads: "
                  << thread_count << '\n';

        std::cout << std::fixed << std::setprecision(3);

        std::cout << "\n--- Performance Metrics ---\n"
                  << "Scan time: " << scan_ms << " ms\n"
                  << "Search time: " << search_ms << " ms\n"
                  << "Total processing time: "
                  << total_ms << " ms\n";
    }

    return result.errors.empty() ? 0 : 1;
}
