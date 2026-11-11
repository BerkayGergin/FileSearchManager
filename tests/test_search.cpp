#include "search.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>

struct ExpectedLine {
    std::size_t number;
    std::string text;
};

struct SearchCase {
    std::string name;
    std::string path;
    std::string query;
    std::size_t files_scanned;
    std::vector<ExpectedLine> lines;
    std::vector<std::string> errors;
};

bool run_case(const SearchCase& test) {
    const SearchResult result = search_file(test.path, test.query);
    bool passed = true;

    if (result.files_scanned != test.files_scanned) {
        std::cerr << test.name << ": expected " << test.files_scanned
                  << " scanned files, got " << result.files_scanned << '\n';
        passed = false;
    }

    if (result.errors != test.errors) {
        std::cerr << test.name << ": expected errors:";
        for (const std::string& error : test.errors) {
            std::cerr << " [" << error << ']';
        }
        std::cerr << ", got:";
        for (const std::string& error : result.errors) {
            std::cerr << " [" << error << ']';
        }
        std::cerr << '\n';
        passed = false;
    }

    if (result.matches.size() != test.lines.size()) {
        std::cerr << test.name << ": expected " << test.lines.size()
                  << " matching lines, got " << result.matches.size() << '\n';
        return false;
    }

    for (std::size_t i = 0; i < test.lines.size(); ++i) {
        const Match& actual = result.matches[i];
        const ExpectedLine& expected = test.lines[i];
        if (actual.file_path != test.path || actual.line_number != expected.number
            || actual.line_text != expected.text) {
            std::cerr << test.name << ": expected " << test.path << ':'
                      << expected.number << ':' << expected.text
                      << ", got " << actual.file_path << ':'
                      << actual.line_number << ':' << actual.line_text << '\n';
            passed = false;
        }
    }

    return passed;
}

int main() {
    const std::string matches = "samples/zehra_match.txt";
    const std::string no_match = "samples/zehra_no_match.txt";
    const std::string empty = "samples/zehra_empty.txt";
    const std::string no_newline = "samples/zehra_no_newline.txt";
    const std::string missing = "samples/zehra_missing.txt";
    const std::vector<SearchCase> cases = {
        {"one result per matching line", matches, "memory", 1,
            {{2, "memory management"}, {3, "shared memory"}, {4, "memory memory"}}, {}},
        {"case-sensitive match", matches, "Memory", 1, {{5, "Memory model"}}, {}},
        {"case-sensitive non-match", no_match, "memory", 1, {}, {}},
        {"query with spaces", matches, "shared memory", 1, {{3, "shared memory"}}, {}},
        {"substring within a word", matches, "emory man", 1, {{2, "memory management"}}, {}},
        {"absent query", matches, "disk", 1, {}, {}},
        {"query cannot cross a line break", matches, "systems\nmemory", 1, {}, {}},
        {"blank lines count toward line numbers", "tests/fixtures/blank_lines.txt", "memory",
            1, {{3, "memory management"}, {5, "shared memory"}}, {}},
        {"empty file", empty, "memory", 1, {}, {}},
        {"last line without newline", no_newline, "memory", 1,
            {{2, "memory management"}}, {}},
        {"empty query", matches, "", 0, {}, {"Arama ifadesi bos olamaz."}},
        {"file open error", missing, "memory", 0, {}, {"Dosya acilamadi: " + missing}},
        {"file read error", "samples", "memory", 0, {},
            {"Dosya okunurken hata olustu: samples"}},
    };

    std::size_t passed = 0;
    for (const SearchCase& test : cases) {
        if (run_case(test)) {
            std::cout << "PASS " << test.name << '\n';
            ++passed;
        }
    }

    std::cout << passed << '/' << cases.size() << " search tests passed.\n";
    return passed == cases.size() ? EXIT_SUCCESS : EXIT_FAILURE;
}
