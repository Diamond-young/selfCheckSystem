#include "logger.h"
#include <sstream>

// 静态成员初始化
std::mutex Logger::log_mutex;
Logger::LogLevel Logger::min_log_level = Logger::INFO;

void Logger::initialize(LogLevel min_level) {
    min_log_level = min_level;
}

std::string Logger::get_current_time() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Logger::level_to_string(LogLevel level) {
    switch (level) {
        case DEBUG: return "DEBUG";
        case INFO: return "INFO";
        case WARNING: return "WARNING";
        case ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < min_log_level) return;
    
    std::lock_guard<std::mutex> lock(log_mutex);
    
    std::string time_str = get_current_time();
    std::string level_str = level_to_string(level);
    
    std::cout << "[" << time_str << "] "
              << "[" << level_str << "] "
              << message << std::endl;
}

void Logger::debug(const std::string& message) {
    log(DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(INFO, message);
}

void Logger::warning(const std::string& message) {
    log(WARNING, message);
}

void Logger::error(const std::string& message) {
    log(ERROR, message);
}