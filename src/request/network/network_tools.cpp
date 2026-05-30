#include "network_tools.hpp"
#include "../../ui/spinner.hpp"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <thread>
#include <mutex>

void NetworkTools::scanPorts(const std::string& targetIp) {
    // Windows Soket Yapısını Başlatıyoruz (Gerekli)
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "\n\033[31m[-] WSAStartup basarisiz oldu.\033[0m\n";
        return;
    }

    Spinner spinner;
    spinner.start("Portlar taraniyor, lutfen bekleyin");

    // Test için listeyi biraz genişletelim ve yaygın portları tarayalım
    std::vector<int> commonPorts = {21, 22, 23, 25, 53, 80, 110, 135, 139, 443, 445, 1433, 3306, 3389, 8080};
    std::vector<int> openPorts;
    std::mutex mtx;

    auto checkPort = [&](int port) {
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock == INVALID_SOCKET) return;

        // Bağlantı zaman aşımı (Timeout) ayarı - 1 saniye
        DWORD timeout = 1000;
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));

        sockaddr_in clientService;
        clientService.sin_family = AF_INET;
        inet_pton(AF_INET, targetIp.c_str(), &clientService.sin_addr);
        clientService.sin_port = htons(port);

        if (connect(sock, (SOCKADDR*)&clientService, sizeof(clientService)) != SOCKET_ERROR) {
            std::lock_guard<std::mutex> lock(mtx);
            openPorts.push_back(port);
        }
        closesocket(sock);
    };

    std::vector<std::thread> threads;
    for (int port : commonPorts) {
        threads.emplace_back(checkPort, port);
    }

    for (auto& th : threads) {
        if (th.joinable()) th.join();
    }

    spinner.stop();

    std::cout << "\n\033[32m[+] Tarama Tamamlandi! Halka Acik acik Portlar (" << targetIp << "):\033[0m\n";
    if (openPorts.empty()) {
        std::cout << "  Hic acik port bulunamadi.\n";
    } else {
        for (int port : openPorts) {
            std::cout << "  -> Port " << port << " [ACIK]\n";
        }
    }

    WSACleanup(); // Soket yapısını kapatıyoruz
}

void NetworkTools::resolveDns(const std::string& domain) {
    // Windows Soket Yapısını Başlatıyoruz (Kritik Eksiklik Buydu)
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "\n\033[31m[-] WSAStartup basarisiz oldu.\033[0m\n";
        return;
    }

    Spinner spinner;
    spinner.start("DNS kayitlari sorgulaniyor");

    struct addrinfo hints {}, *result = nullptr;
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(domain.c_str(), nullptr, &hints, &result) != 0) {
        spinner.stop();
        std::cout << "\n\033[31m[-] DNS cozulemedi: " << domain << "\033[0m\n";
        WSACleanup();
        return;
    }

    spinner.stop();
    std::cout << "\n\033[32m[+] DNS Sonuclari (" << domain << "):\033[0m\n";
    
    for (struct addrinfo* ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
        char ipstr[INET_ADDRSTRLEN];
        void* addr = &((struct sockaddr_in*)ptr->ai_addr)->sin_addr;
        inet_ntop(ptr->ai_family, addr, ipstr, sizeof(ipstr));
        std::cout << "  -> IP Adresi: " << ipstr << "\n";
    }

    freeaddrinfo(result);
    WSACleanup(); // Soket yapısını kapatıyoruz
}