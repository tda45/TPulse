#ifndef IP_HANDLER_HPP
#define IP_HANDLER_HPP

#include <string>

namespace Request {
    class IpHandler {
    public:
        static void Execute();
    private:
        static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    };
}

#endif // IP_HANDLER_HPP