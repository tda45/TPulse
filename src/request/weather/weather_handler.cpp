#include "weather_handler.hpp"
#include "../../network/http_client.hpp"
#include "../history/history_handler.hpp"
#include "../../common/theme.hpp"
#include <iostream>
#include <map>

void Request::WeatherHandler::Execute(const std::string& city) {
    HttpClient client;
    
    Request::HistoryHandler::LogAction("WEATHER", "SUCCESS");

    std::string url = "https://wttr.in/" + city;
    
    std::cout << Theme::TEXT_GRAY << "[*] Hava durumu verisi cekiliyor... " << Theme::CRIMSON << (city.empty() ? "Mevcut Konum" : city) << Theme::RESET << "\n";
    
    // HİLE: wttr.in'i kandırıp curl olduğumuzu söylüyoruz
    std::map<std::string, std::string> headers;
    headers["User-Agent"] = "curl/7.88.1";

    // İstek atarken hazırladığımız başlıkları bota yediriyoruz
    HttpResponse res = client.SendRequest(url, "GET", "", headers);

    std::cout << Theme::ANTHRACITE << "--------------------------------------------------" << Theme::RESET << "\n";
    
    if (res.statusCode == 200) {
        // Gelen veri artık HTML değil, saf ve renkli terminal tablosu!
        std::cout << res.body << "\n";
    } else {
        std::cout << Theme::BRIGHT_RED << "[!] Hava durumu alinamadi. Durum kodu: " << res.statusCode << Theme::RESET << "\n";
    }
    std::cout << Theme::ANTHRACITE << "--------------------------------------------------" << Theme::RESET << "\n";
}