#ifndef CRYPTO_HANDLER_HPP
#define CRYPTO_HANDLER_HPP

#include <string>
#include <curl/curl.h> // CURL tipi için ekledik

namespace Request {
    class CryptoHandler {
    public:
        static void Execute();
    private:
        static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
        static std::string ExtractValue(const std::string& json, const std::string& key);
        // UNUTTUĞUMUZ SATIR:
        static void FetchCoinData(CURL* curl, const std::string& coinId, const std::string& label);
    };
}

#endif // CRYPTO_HANDLER_HPP