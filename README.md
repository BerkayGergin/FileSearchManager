FileSearchManager V2
FileSearchManager, C++17 ile geliştirilen, dosyaların içindeki metinleri çoklu iş parçacığı (thread) kullanarak arayan bir uygulamadır. V2 sürümünde gelişmiş dosya filtreleri, JSON çıktısı, süre ölçümü ve Streamlit tabanlı bir grafik arayüz bulunur.
Takım ve modüller
Sorumlu	Modül	Görev
Berkay	src/file_scanner.cpp, include/file_scanner.hpp, src/main.cpp, gui/app.py	Dosya keşfi ve filtreleme, komut satırı seçenekleri, JSON çıktısı, grafik arayüz entegrasyonu
Zehra	src/search.cpp, include/search.hpp	Tek dosyada metin arama ve ilgili testler
Umut	src/parallel_search.cpp, include/parallel_search.hpp	Thread tabanlı paralel arama, eşzamanlılık güvenliği ve performans değerlendirmesi


Özellikler
- Varsayılan olarak .txt dosyalarını arar; --extensions ile .csv, .md, .log, .json gibi başka düz metin uzantıları seçilebilir.
- --recursive ile alt klasörler taranabilir.
- Dosya adında ifade, minimum/maksimum dosya boyutu ve tek tek seçilen dosyalarla kapsam daraltılabilir.
- 1–8 thread arasında arama yapılabilir.
- Sonuçlar dosya yolu ve satır numarasına göre sıralanır.
- JSON çıktı ve tarama/arama/toplam işlem süresi metrikleri sağlar.
- Streamlit arayüzü, C++ programını çalıştırıp JSON sonuçlarını tabloda gösterir.
Not: .csv, .md, .log, .json uzantıları metin olarak taranır; yapılandırılmış içerikleri özel olarak ayrıştırılmaz. PDF ve DOCX içerik çıkarımı bu sürümün kapsamında değildir.
Gereksinimler
- C++17 uyumlu g++ derleyicisi ve pthread desteği
- Grafik arayüz için Python 3 ve Streamlit
- Windows üzerinde geliştirme için Ubuntu WSL kullanılabilir
C++ programını derleme
Ubuntu/WSL terminalinde proje kök dizininde:
mkdir -p build
g++ -std=c++17 -Wall -Wextra -Wpedantic -pthread -Iinclude \
  src/main.cpp src/file_scanner.cpp src/search.cpp src/parallel_search.cpp \
  -o build/filesearch
Komut satırından çalıştırma
Temel kullanım:
./build/filesearch DIRECTORY QUERY THREADS [OPTIONS]
Örnekler:
./build/filesearch samples memory 2
./build/filesearch samples memory 2 --filename zehra
./build/filesearch samples memory 2 --extensions .txt,.csv,.md --recursive
./build/filesearch samples memory 2 --min-kb 1 --max-kb 100
./build/filesearch samples memory 2 --file a.txt
./build/filesearch samples memory 2 --json
./build/filesearch samples memory 2 --quiet
Desteklenen seçenekler:
Seçenek	Açıklama
--extensions .txt,.csv	Taranacak dosya uzantıları
--recursive	Alt klasörleri de tara
--filename TEXT	Dosya adında geçen ifadeye göre filtrele
--min-kb N	Minimum dosya boyutu, KB
--max-kb N	Maksimum dosya boyutu, KB
--file RELATIVE_PATH	Yalnızca belirli dosyayı dahil et; tekrarlanabilir
--json	Arama sonucunu JSON olarak yazdır
--quiet	Normal çıktıda eşleşme satırlarını gizle


THREADS değeri 1 ile 8 arasında olmalıdır. --file için yol, aranan klasöre göre görelidir.
JSON çıktısı
--json çıktısındaki başlıca alanlar:
- files_discovered, files_scanned, matching_lines, threads
- scan_ms, search_ms, total_ms
- matches: file_path, line_number, line_text
- errors: varsa hata mesajları
Örnek verilerle samples memory 2 aramasında beklenen özet 7 taranan dosya ve 6 eşleşmedir. Süreler donanıma ve çalıştırma koşullarına göre değişir.
Streamlit grafik arayüzü
Ubuntu/WSL'de bir Python sanal ortamı oluşturun (ilk kurulumda):
python3 -m venv ~/venvs/filesearch-v2
source ~/venvs/filesearch-v2/bin/activate
python -m pip install streamlit
Sonraki çalıştırmalarda:
source ~/venvs/filesearch-v2/bin/activate
python -m streamlit run gui/app.py --server.headless true
Tarayıcıda http://localhost:8501 adresini açın.

Önemli: Arayüzün C++ uygulamasını bulması için build/filesearch ikilisi derlenmiş olmalıdır. Arayüz, mevcutsa /tmp/filesearch_berkay_v2 yolundaki ikiliyi öncelikli kullanır; bu nedenle eski derlemenin kullanılmaması için bu ikiliyi de güncel tutun veya kaldırın. Arayüzü WSL ortamında çalıştırırken klasör yollarını WSL biçiminde girin (samples veya /mnt/c/...).
Testler
Dosya tarama birim testleri
g++ -std=c++17 -Wall -Wextra -Wpedantic -pthread -Iinclude \
  tests/test_file_scanner.cpp src/file_scanner.cpp \
  -o /tmp/test_file_scanner
/tmp/test_file_scanner
Bu testler varsayılan TXT tarama, çoklu uzantı, özyinelemeli tarama, dosya adı filtresi, seçili dosyalar, boyut filtresi ve geçersiz klasör durumlarını kapsar.
Temel entegrasyon testi
./build/filesearch samples memory 1
./build/filesearch samples memory 2
./build/filesearch samples memory 4
./build/filesearch samples memory 2 --json | python3 -m json.tool
Beklenen örnek sonuç, thread sayısından bağımsız olarak 7 dosya ve 6 eşleşmedir. Farklı thread sayılarındaki süreler tek çalıştırmayla doğrudan kıyaslanmamalıdır.
Arayüz doğrulaması
1. samples, memory, 2 thread, .txt için 7 dosya / 6 eşleşme beklenir.
2. Dosya adı filtresine zehra girildiğinde 4 dosya / 4 eşleşme beklenir.
3. Yalnızca a.txt seçildiğinde 1 dosya / 1 eşleşme beklenir.
Proje akışı
1. file_scanner filtrelere uygun dosyaları belirler.
2. parallel_search dosyaları worker thread'ler arasında dağıtır.
3. search her dosyada eşleşen satırları bulur.
4. main sonuçları sıralar ve terminal/JSON çıktısı üretir.
5. gui/app.py C++ uygulamasını çağırır, JSON'u okur ve tabloda gösterir.
Demo 2
Demo sırasında çalışır C++ çekirdek mantığı, modül entegrasyonu, en az iki test (kenar durumları dahil), çıktılar ve süre metrikleri, Git geçmişi ve grafik arayüz gösterilebilir.
Entegrasyon notu: Takımın PR'ları birleştirildikten sonra, main dalında yeniden derleme ve testlerin başarıyla çalıştığı ayrıca doğrulanmalıdır. Bu README, doğrulanmamış son testleri geçmiş gibi göstermemektedir.
