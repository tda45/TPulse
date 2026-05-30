#include "post_handler.hpp"
#include "../common/theme.hpp"
#include <iostream>

void Request::PostHandler::Execute(const std::string& url, const std::string& payload) {
    HttpClient client;
    
    std::cout << Theme::TEXT_GRAY << "[*] Veri gonderiliyor: " << Theme::CRIMSON << url << Theme::RESET << "\n";
    
    HttpResponse res = client.SendRequest(url, "POST", payload);

    std::cout << Theme::ANTHRACITE << "--------------------------------------------------" << Theme::RESET << "\n";
    
    // HTTP Durumuna Göre Renk Ataması (200 OK ise kırmızı/beyaz, hata ise parlak kırmızı)
    std::string statusColor = (res.statusCode == 200) ? Theme::CRIMSON : Theme::BRIGHT_RED;
    
    std::cout << Theme::TEXT_GRAY << "HTTP STATUS: " << statusColor << res.statusCode << Theme::RESET << "\n";
    std::cout << Theme::TEXT_GRAY << "SURE       : " << Theme::CRIMSON << res.elapsedMs << " ms" << Theme::RESET << "\n";
    
    std::cout << Theme::ANTHRACITE << "--------- [ RESPONSE BODY ] --------- " << Theme::RESET << "\n";
    if (!res.body.empty()) {
        std::cout << Theme::TEXT_GRAY << res.body << Theme::RESET << "\n";
    } else {
        std::cout << Theme::BRIGHT_RED << "< Bos Boş veya Hatali Yanit >" << Theme::RESET << "\n";
    }
    std::cout << Theme::ANTHRACITE << "--------------------------------------------------" << Theme::RESET << "\n";
}