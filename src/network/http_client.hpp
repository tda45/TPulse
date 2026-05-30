#pragma once
#include <windows.h>
#include <winhttp.h>
#include <string>
#include <vector>
#include <map>

#pragma comment(lib, "winhttp.lib")

struct HttpResponse {
    int statusCode = 0;
    std::string body;
    std::string headers;
    double elapsedMs = 0.0;
};

class HttpClient {
public:
    HttpClient();
    ~HttpClient();

    // extraHeaders parametresi eklendi (Varsayılan olarak boş)
    HttpResponse SendRequest(const std::string& url, const std::string& method, const std::string& data = "", const std::map<std::string, std::string>& extraHeaders = {});

private:
    HINTERNET hSession = nullptr;
    bool ParseUrl(const std::string& url, std::wstring& outHost, std::wstring& outPath, INTERNET_PORT& outPort, BOOL& outIsHttps);
};