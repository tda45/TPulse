#include "progress_bar.hpp"
#include <iostream>
#include <iomanip>

void ProgressBar::update(double percentage, size_t downloadedBytes, size_t totalBytes) {
    int barWidth = 40;
    int pos = static_cast<int>(barWidth * (percentage / 100.0));

    // \033[31m -> Kırmızı başlatır | \033[0m -> Rengi sıfırlar
    std::cout << "\r\033[31m[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    
    // Boyutları MB cinsinden hesaplayalım
    double downloadedMB = downloadedBytes / (1024.0 * 1024.0);
    double totalMB = totalBytes / (1024.0 * 1024.0);

    std::cout << "] " << std::fixed << std::setprecision(1) << percentage << "% "
              << "(" << downloadedMB << " MB / " << totalMB << " MB)\033[0m" << std::flush;
}