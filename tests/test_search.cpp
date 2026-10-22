#include <iostream>
#include "search.hpp"

// Aramayi calistir ve donen sonucu ekrana yaz.
void run_test(const std::string& path) {
    std::cout << "\n--- " << path << " ---\n";

    SearchResult result = search_file(path, "memory");

    for (const Match& match : result.matches) {
        std::cout << match.file_path << ":"
                  << match.line_number << ":"
                  << match.line_text << "\n";
    }

    for (const std::string& error : result.errors) {
        std::cout << "Hata: " << error << "\n";
    }

    std::cout << "Basariyla okunan dosya: "
              << result.files_scanned << "\n";

    std::cout << "Eslesen satir: "
              << result.matches.size() << "\n";
}

int main() {
    // Eslesme bulunan dosya.
    run_test("samples/zehra_match.txt");

    // Eslesme bulunmayan dosya.
    run_test("samples/zehra_no_match.txt");

    // Olusturmadigimiz dosya: acilma hatasini deniyoruz.
    run_test("samples/zehra_missing.txt");
    // Bos Dosya.
    run_test("samples/zehra_empty.txt");
    // Son satirinin sonunda yeni satir satir karakteri olmayan dosya.
    run_test("samples/zehra_no_newline.txt");
    return 0;
}
