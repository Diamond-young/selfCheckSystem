#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <mutex>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

class Logger {
public:
    enum LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };
    
    // 初始化日志系统
    static void initialize(LogLevel min_level = INFO);
    
    // 记录日志
    static void log(LogLevel level, const std::string& message);
    
    // 快捷方法
    static void debug(const std::string& message);
    static void info(const std::string& message);
    static void warning(const std::string& message);
    static void error(const std::string& message);
    
private:
    static std::mutex log_mutex;
    static LogLevel min_log_level;
    
    // 获取当前时间字符串
    static std::string get_current_time();
    
    // 日志级别字符串
    static std::string level_to_string(LogLevel level);
};

#endif // LOGGER_H