#include "history_handler.hpp"
#include "../../common/theme.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>

namespace Request {

    // Log dosyasının adı
    const std::string HistoryHandler::g_HistoryFile = "tpulse_history.log";

    std::string HistoryHandler::GetCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm* local_time = std::localtime(&now_time);

        char buffer[20];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", local_time);
        return std::string(buffer);
    }

    void HistoryHandler::LogAction(const std::string& command, const std::string& status) {
        // ios::app -> Dosyanın sonuna ekleme (append) modunda açar
        std::ofstream outFile(g_HistoryFile, std::ios::app);
        if (!outFile) return;

        // Her satırı: [Zaman] | Komut | Durum formatında yazar
        outFile << "[" << GetCurrentTimestamp() << "] | " << command << " | " << status << "\n";
        outFile.close();
    }

    void HistoryHandler::Execute() {
        std::ifstream inFile(g_HistoryFile);
        
        std::cout << Theme::TEXT_GRAY << "[*] T-PULSE Islem gecmisi yukleniyor...\n" << Theme::RESET;
        std::cout << Theme::TEXT_GRAY << "--------------------------------------------------\n" << Theme::RESET;
        std::cout << Theme::RESET << "📜 ISLEM GECMISI (SON ISLEMLER)\n";
        std::cout << Theme::TEXT_GRAY << "==================================================\n" << Theme::RESET;

        if (!inFile) {
            std::cout << "  Henuz hicbir islem kaydi bulunmuyor.\n";
            std::cout << Theme::TEXT_GRAY << "--------------------------------------------------\n" << Theme::RESET;
            return;
        }

        std::string line;
        std::vector<std::string> allLogs;

        // Tüm logları oku ve vektöre at (Son yapılanları en üstte göstermek için)
        while (std::getline(inFile, line)) {
            if (!line.empty()) {
                allLogs.push_back(line);
            }
        }
        inFile.close();

        if (allLogs.empty()) {
            std::cout << "  Henuz hicbir islem kaydi bulunmuyor.\n";
        } else {
            // Son 10 logu ters sırada (en güncel en üstte) ekrana bas
            int count = 0;
            for (auto it = allLogs.rbegin(); it != allLogs.rend() && count < 10; ++it, ++count) {
                // Ham log satırını parçalayarak daha şık basabiliriz
                std::string log = *it;
                size_t firstPipe = log.find('|');
                size_t secondPipe = log.find('|', firstPipe + 1);

                if (firstPipe != std::string::npos && secondPipe != std::string::npos) {
                    std::string timePart = log.substr(0, firstPipe - 1);
                    std::string cmdPart = log.substr(firstPipe + 2, secondPipe - firstPipe - 3);
                    std::string statusPart = log.substr(secondPipe + 2);

                    std::string statusColor = (statusPart == "SUCCESS") ? Theme::RESET : Theme::BRIGHT_RED;

                    std::cout << Theme::TEXT_GRAY << timePart << Theme::RESET 
                              << " -> " << Theme::CRIMSON << std::left << std::setw(10) << cmdPart << Theme::RESET
                              << " [" << statusColor << statusPart << Theme::RESET << "]\n";
                } else {
                    // Eğer format bozuksa direkt satırı bas
                    std::cout << "  " << log << "\n";
                }
            }
        }

        std::cout << Theme::TEXT_GRAY << "--------------------------------------------------\n" << Theme::RESET;
    }
}