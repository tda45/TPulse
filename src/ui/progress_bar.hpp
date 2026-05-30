#pragma once
#include <string>

class ProgressBar {
public:
    static void update(double percentage, size_t downloadedBytes, size_t totalBytes);
};