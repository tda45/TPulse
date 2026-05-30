#pragma once
#include <string>
#include <vector>

namespace Request {

    struct LogEntry {
        std::string timestamp;
        std::string command;
        std::string status;
    };

    class HistoryHandler {
    public:
        // Yeni bir işlemi geçmişe kaydeder
        static void LogAction(const std::string& command, const std::string& status = "SUCCESS");
        
        // Geçmişi terminale şık bir tablo olarak basar
        static void Execute();

    private:
        static const std::string g_HistoryFile;
        
        // Şu anki zamanı [YYYY-MM-DD HH:MM:SS] formatında string olarak döner
        static std::string GetCurrentTimestamp();
    };
}