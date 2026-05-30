#include "http_client.hpp"
#include "../common/theme.hpp"
#include <chrono>
#include <iostream>

HttpClient::HttpClient() {
    hSession = WinHttpOpen(L"T-Pulse Engine/1.0", 
                           WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, 
                           WINHTTP_NO_PROXY_NAME, 
                           WINHTTP_NO_PROXY_BYPASS, 0);
}

HttpClient::~HttpClient() {
    if (hSession) WinHttpCloseHandle(hSession);
}

bool HttpClient::ParseUrl(const std::string& url, std::wstring& outHost, std::wstring& outPath, INTERNET_PORT& outPort, BOOL& outIsHttps) {
    URL_COMPONENTS urlComp = { 0 };
    urlComp.dwStructSize = sizeof(urlComp);
    
    std::wstring wUrl(url.begin(), url.end());
    
    urlComp.dwSchemeLength = (DWORD)-1;
    urlComp.dwHostNameLength = (DWORD)-1;
    urlComp.dwUrlPathLength = (DWORD)-1;

    if (!WinHttpCrackUrl(wUrl.c_str(), (DWORD)wUrl.length(), 0, &urlComp)) return false;

    outHost = std::wstring(urlComp.lpszHostName, urlComp.dwHostNameLength);
    outPath = std::wstring(urlComp.lpszUrlPath, urlComp.dwUrlPathLength);
    outPort = urlComp.nPort;
    outIsHttps = (urlComp.nScheme == INTERNET_SCHEME_HTTPS);
    return true;
}

HttpResponse HttpClient::SendRequest(const std::string& url, const std::string& method, const std::string& data, const std::map<std::string, std::string>& extraHeaders) {
    HttpResponse response;
    std::wstring host, path;
    INTERNET_PORT port;
    BOOL isHttps;

    if (!ParseUrl(url, host, path, port, isHttps)) {
        std::cerr << Theme::BRIGHT_RED << "[!] Gecersiz URL formati." << Theme::RESET << std::endl;
        return response;
    }

    auto start = std::chrono::high_resolution_clock::now();

    HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(), port, 0);
    if (!hConnect) return response;

    std::wstring wMethod(method.begin(), method.end());
    DWORD flags = isHttps ? WINHTTP_FLAG_SECURE : 0;

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, wMethod.c_str(), path.c_str(), 
                                            NULL, WINHTTP_NO_REFERER, 
                                            WINHTTP_DEFAULT_ACCEPT_TYPES, flags);

    if (hRequest) {
        // Başlıkları dinamik olarak inşa et
        std::wstring headers = L"Content-Type: application/json\r\n";
        for (const auto& [key, value] : extraHeaders) {
            std::wstring wKey(key.begin(), key.end());
            std::wstring wValue(value.begin(), value.end());
            headers += wKey + L": " + wValue + L"\r\n";
        }
        
        BOOL results = WinHttpSendRequest(hRequest, headers.c_str(), (DWORD)-1L, 
                                          (LPVOID)data.c_str(), (DWORD)data.length(), 
                                          (DWORD)data.length(), 0);

        if (results) results = WinHttpReceiveResponse(hRequest, NULL);

        if (results) {
            DWORD statusCode = 0;
            DWORD dwSize = sizeof(statusCode);
            WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, 
                                WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &dwSize, WINHTTP_NO_HEADER_INDEX);
            response.statusCode = statusCode;

            DWORD dwAvailableSize = 0;
            do {
                if (!WinHttpQueryDataAvailable(hRequest, &dwAvailableSize)) break;
                if (dwAvailableSize == 0) break;

                std::vector<char> buffer(dwAvailableSize + 1);
                DWORD dwReceivedBytes = 0;

                if (WinHttpReadData(hRequest, buffer.data(), dwAvailableSize, &dwReceivedBytes)) {
                    buffer[dwReceivedBytes] = '\0';
                    response.body.append(buffer.data(), dwReceivedBytes);
                }
            } while (dwAvailableSize > 0);
        }
        WinHttpCloseHandle(hRequest);
    }
    WinHttpCloseHandle(hConnect);

    auto end = std::chrono::high_resolution_clock::now();
    response.elapsedMs = std::chrono::duration<double, std::milli>(end - start).count();

    return response;
}