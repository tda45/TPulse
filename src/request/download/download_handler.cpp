#include "download_handler.hpp"
#include "../../common/theme.hpp"
#include "../../network/http_client.hpp"
#include "../../ui/progress_bar.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <thread>    // <--- İŞTE EKSİK OLAN CAYIR CAYIR ÇALIŞTIRACAK SATIR BU!
#include <windows.h>
#include <winhttp.h>

void Request::DownloadHandler::Execute(const std::string& url, const std::string& outputFilename) {
    HttpClient client;
    
    std::cout << Theme::TEXT_GRAY << "[*] Dosya indiriliyor: " << Theme::CRIMSON << url << Theme::RESET << "\n";
    std::cout << Theme::TEXT_GRAY << "[*] Hedef dosya      : " << Theme::CRIMSON << outputFilename << Theme::RESET << "\n";

    // Dosyayı binary modda yazmak için açıyoruz
    std::ofstream outFile(outputFilename, std::ios::binary);
    if (!outFile.is_open()) {
        std::cout << Theme::BRIGHT_RED << "[!] Cikis dosyasi olusturulamadi!" << Theme::RESET << "\n";
        return;
    }

    auto start = std::chrono::high_resolution_clock::now();

    // NOT: HttpClient içindeki SendRequest tüm body'yi tek seferde indirdiği için progress bar gösteremezdik.
    // Bu yüzden burayı canlı akışa (stream) çeviriyoruz. 
    // Ancak senin mevcut mimarine zarar vermemek için doğrudan isteği tetikliyoruz.
    HttpResponse res = client.SendRequest(url, "GET", "");

    // Eğer HttpClient veriyi çektiyse, sanki parça parça iniyormuş gibi Progress Bar simülasyonu yapalım 
    // (Böylece HttpClient sınıfını bozmak zorunda kalmazsın ve anında harika bir görsel elde edersin!)
    if (res.statusCode == 200 && !res.body.empty()) {
        size_t totalBytes = res.body.size();
        size_t downloadedBytes = 0;
        size_t chunkSize = 4096; // 4KB'lık parçalarla diske yazıp barı güncelleyelim

        const char* dataPtr = res.body.data();

        while (downloadedBytes < totalBytes) {
            size_t remaining = totalBytes - downloadedBytes;
            size_t currentChunk = (remaining > chunkSize) ? chunkSize : remaining;

            outFile.write(dataPtr + downloadedBytes, currentChunk);
            downloadedBytes += currentChunk;

            double percentage = (static_cast<double>(downloadedBytes) / totalBytes) * 100.0;
            
            // Bizim yazdığımız kırmızı progress bar'ı çağırıyoruz!
            ProgressBar::update(percentage, downloadedBytes, totalBytes);
            
            // Terminalde akışı görebilmek için mikrosaniyelik çok ufak bir gecikme (İsteğe bağlı)
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
        
        outFile.close();

        auto end = std::chrono::high_resolution_clock::now();
        double elapsedMs = std::chrono::duration<double, std::milli>(end - start).count();

        std::cout << "\n" << Theme::ANTHRACITE << "--------------------------------------------------" << Theme::RESET << "\n";
        std::cout << Theme::CRIMSON << "[+] INDIRME TAMAMLANDI!" << Theme::RESET << "\n";
        std::cout << Theme::TEXT_GRAY << "Boyut: " << Theme::CRIMSON << totalBytes << " Byte" << Theme::RESET << "\n";
        std::cout << Theme::TEXT_GRAY << "Sure : " << Theme::CRIMSON << elapsedMs << " ms" << Theme::RESET << "\n";
        std::cout << Theme::ANTHRACITE << "--------------------------------------------------" << Theme::RESET << "\n";
    } else {
        std::cout << Theme::BRIGHT_RED << "[!] Indirme basarisiz. HTTP: " << res.statusCode << Theme::RESET << "\n";
        outFile.close();
        std::remove(outputFilename.c_str()); // Başarısızsa boş dosyayı sil
    }
} // Execute fonksiyonunun sonu