#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>

enum LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3
};

class Logger {
public:
    static LogLevel level;

    static void setLevel(LogLevel l) { level = l; }

    static void debug(const std::string& msg) {
        if(level <= DEBUG) std::cout << "[DEBUG] " << msg << std::endl;
    }

    static void info(const std::string& msg) {
        if(level <= INFO) std::cout << "[INFO] " << msg << std::endl;
    }

    static void warn(const std::string& msg) {
        if(level <= WARN) std::cout << "[WARN] " << msg << std::endl;
    }

    static void error(const std::string& msg) {
        if(level <= ERROR) std::cerr << "[ERROR] " << msg << std::endl;
    }
};

LogLevel Logger::level = INFO;

#endif
