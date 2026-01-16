#include "logger.h"
#include <iostream>

Logger::Logger(const std::string& path) {
    file_.open(path, std::ios::app);
    if (!file_) {
        std::cerr << "Failed to open log file\n";
    }
}

Logger& Logger::instance() {
    static Logger logger("server.log");
    return logger;
}

