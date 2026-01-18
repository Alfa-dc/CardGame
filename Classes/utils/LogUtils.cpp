// LogUtils.cpp
// 日志工具类实现
//
// 这个文件实现了LogUtils类的所有方法，提供统一的日志输出功能
//
#include "LogUtils.h"
#include <stdarg.h>
#include <cstdio>
#include <ctime>
#include <cstdlib>

namespace utils {

// 初始化当前日志级别为DEBUG
LogUtils::LogLevel LogUtils::_currentLevel = LogUtils::LogLevel::DEBUG;

// 构造函数
LogUtils::LogUtils() {
}

// 设置日志级别
void LogUtils::setLogLevel(LogLevel level) {
    _currentLevel = level;
}

// 输出调试日志
void LogUtils::debug(const std::string& message) {
    if (_currentLevel <= LogLevel::DEBUG) {
        log(LogLevel::DEBUG, message);
    }
}

// 输出普通信息日志
void LogUtils::info(const std::string& message) {
    if (_currentLevel <= LogLevel::INFO) {
        log(LogLevel::INFO, message);
    }
}

// 输出警告日志
void LogUtils::warning(const std::string& message) {
    if (_currentLevel <= LogLevel::WARNING) {
        log(LogLevel::WARNING, message);
    }
}

// 输出错误日志
void LogUtils::error(const std::string& message) {
    if (_currentLevel <= LogLevel::ERROR) {
        log(LogLevel::ERROR, message);
    }
}

// 输出带有格式的调试日志
void LogUtils::debugFormat(const char* format, ...) {
    if (_currentLevel <= LogLevel::DEBUG) {
        va_list args;
        va_start(args, format);
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        log(LogLevel::DEBUG, buffer);
    }
}

// 输出带有格式的普通信息日志
void LogUtils::infoFormat(const char* format, ...) {
    if (_currentLevel <= LogLevel::INFO) {
        va_list args;
        va_start(args, format);
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        log(LogLevel::INFO, buffer);
    }
}

// 输出带有格式的警告日志
void LogUtils::warningFormat(const char* format, ...) {
    if (_currentLevel <= LogLevel::WARNING) {
        va_list args;
        va_start(args, format);
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        log(LogLevel::WARNING, buffer);
    }
}

// 输出带有格式的错误日志
void LogUtils::errorFormat(const char* format, ...) {
    if (_currentLevel <= LogLevel::ERROR) {
        va_list args;
        va_start(args, format);
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        log(LogLevel::ERROR, buffer);
    }
}

// 实际输出日志的函数
void LogUtils::log(LogLevel level, const std::string& message) {
    // 获取当前时间
    time_t now = time(nullptr);
    struct tm* localTime = localtime(&now);
    
    // 格式化时间字符串
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localTime);
    
    // 获取日志级别字符串
    std::string levelStr = levelToString(level);
    
    // 构建完整日志信息
    std::string logMessage = "[";
    logMessage += timeStr;
    logMessage += "] [";
    logMessage += levelStr;
    logMessage += "] ";
    logMessage += message;
    
    // 输出日志到控制台
    printf("%s\n", logMessage.c_str());
}

// 将日志级别转换为字符串
std::string LogUtils::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}

} // namespace utils