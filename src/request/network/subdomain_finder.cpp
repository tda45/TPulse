#include "subdomain_finder.hpp"
#include "../../ui/spinner.hpp"
#include "../../common/theme.hpp"
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <winsock2.h>
#include <ws2tcpip.h>

void Request::SubdomainFinder::Execute(const std::string& targetDomain) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << Theme::BRIGHT_RED << "[-] Windows soket altyapisi baslatilamadi.\n" << Theme::RESET;
        return;
    }

    Spinner spinner;
    spinner.start("Subdomain taramasi yapiliyor, lutfen bekleyin");

    // En yaygın kullanılan subdomain kelime listesi (Wordlist)
    std::vector<std::string> wordlist = {
        "www", "mail", "remote", "blog", "webmail", "server", "ns1", "ns2",
        "smtp", "vpn", "secure", "panel", "admin", "cloud", "api", "dev",
        "staging", "shop", "cpanel", "whm", "test", "ftp", "assets", "db"
    };

    std::vector<std::string> liveSubdomains;
    std::mutex mtx;

    // Her bir subdomaini kontrol eden lambda fonksiyonu
    auto checkSubdomain = [&](const std::string& sub) {
        std::string fullDomain = sub + "." + targetDomain;
        struct addrinfo hints{}, *result = nullptr;
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        // getaddrinfo başarılı dönerse subdomain canlıdır
        if (getaddrinfo(fullDomain.c_str(), nullptr, &hints, &result) == 0) {
            std::lock_guard<std::mutex> lock(mtx);
            liveSubdomains.push_back(fullDomain);
            freeaddrinfo(result);
        }
    };

    // Thread havuzu oluşturup asenkron tarama başlatıyoruz
    std::vector<std::thread> threads;
    for (const auto& sub : wordlist) {
        threads.emplace_back(checkSubdomain, sub);
    }

    for (auto& th : threads) {
        if (th.joinable()) th.join();
    }

    spinner.stop();

    std::cout << "\n" << Theme::CRIMSON << "[+] Tarama Tamamlandi! Bulunan Canli Subdomainler (" << targetDomain << "):\n" << Theme::RESET;
    if (liveSubdomains.empty()) {
        std::cout << Theme::TEXT_GRAY << "  Hiç aktif subdomain bulunamadı.\n" << Theme::RESET;
    } else {
        for (const auto& liveSub : liveSubdomains) {
            std::cout << "  " << Theme::BRIGHT_GREEN << "-> " << liveSub << Theme::RESET << "\n";
        }
    }

    WSACleanup();
}