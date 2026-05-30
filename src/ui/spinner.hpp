#pragma once
#include <atomic>
#include <thread>
#include <string>

class Spinner {
private:
    std::atomic<bool> running{false};
    std::thread spinnerThread;
public:
    // Başına açıkça std::string ekledik
    void start(const std::string& message = std::string("Sistem calisiyor lutfen bekleyin"));
    void stop();
    ~Spinner();
};