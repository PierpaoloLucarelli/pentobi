#pragma once

#include <fstream>
#include <mutex>
#include <iostream>

class Logger {
public:
    static Logger& instance();

    template<typename... Args>
    void info(Args&&... args) {
        log("INFO", std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(Args&&... args) {
        log("ERROR", std::forward<Args>(args)...);
    }

private:
    Logger(const std::string& path);

    template<typename First, typename... Rest>
    void log(const char* level, First&& first, Rest&&... rest) {
        std::lock_guard<std::mutex> lock(mutex_);

        file_ << "[" << level << "] " << first;
        (file_ << ... << rest);
        file_ << std::endl;

        std::cerr << "[" << level << "] " << first;
        (std::cerr << ... << rest);
        std::cerr << std::endl;
    }

    std::ofstream file_;
    std::mutex mutex_;
};

