#include "spinner.hpp"
#include <iostream>
#include <chrono>
#include <thread> // std::this_thread için kesinlikle gerekli

void Spinner::start(const std::string& message) {
    running = true;
    spinnerThread = std::thread([this, message]() {
        const char spinnerChars[] = {'|', '/', '-', '\\'};
        int i = 0;
        while (running) {
            std::cout << "\r[" << spinnerChars[i % 4] << "] " << message << "..." << std::flush;
            i++;
            // Çince karakter temizlendi, std::this_thread eklendi:
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::cout << "\r" << std::string(message.length() + 10, ' ') << "\r" << std::flush;
    });
}

void Spinner::stop() {
    if (running) {
        running = false;
        if (spinnerThread.joinable()) {
            spinnerThread.join();
        }
    }
}

Spinner::~Spinner() {
    stop();
}