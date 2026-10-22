#include "search.hpp"
#include <fstream>

SearchResult search_file(
    const std::string& file_path,
    const std::string& query
) {
    SearchResult result;

    // Bos arama ifadesini reddet.
    if (query.empty()) {
        result.errors.push_back("Arama ifadesi bos olamaz.");
        return result;
    }

    // Dosyayi okumak icin ac.
    std::ifstream file(file_path);

    // Dosya acilamadiysa hatayi kaydet.
    if (!file.is_open()) {
        result.errors.push_back("Dosya acilamadi: " + file_path);
        return result;
    }

    std::string line;
    std::size_t line_number = 0;

    // Her turda bir satir oku.
    while (std::getline(file, line)) {
        ++line_number;

        // Aranan ifade bu satirda varsa sonucu kaydet.
        if (line.find(query) != std::string::npos) {
            result.matches.push_back(
                Match{file_path, line_number, line}
            );
        }
    }

    // Okuma hatasi varsa dosyayi basarili sayma.
    if (file.bad() || (file.fail() && !file.eof())) {
        result.errors.push_back("Dosya okunurken hata olustu: " + file_path);
        return result;
    }

    // Dosyanin tamami basariyla okundu.
    result.files_scanned = 1;

    return result;
}
