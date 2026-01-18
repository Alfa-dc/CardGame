// LogUtils.h
// 日志工具类 - 提供标准化的日志输出功能
//
// 这个类提供了统一的日志输出接口，支持不同级别的日志记录
// 包括：调试信息、普通信息、警告和错误
//
// 核心功能：
// 1. 支持不同级别的日志输出
// 2. 提供统一的日志格式
// 3. 支持开关不同级别的日志
// 4. 便于后续扩展（如添加文件日志、网络日志等）
//
// 使用示例：
// LogUtils::debug("This is a debug message");
// LogUtils::info("This is an info message");
// LogUtils::warning("This is a warning message");
// LogUtils::error("This is an error message");
//
// 可通过宏定义控制日志级别：
// #define LOG_LEVEL_DEBUG - 输出所有日志
// #define LOG_LEVEL_INFO - 输出info、warning、error
// #define LOG_LEVEL_WARNING - 输出warning、error
// #define LOG_LEVEL_ERROR - 只输出error
//
#ifndef __LOG_UTILS_H__
#define __LOG_UTILS_H__

#include <string>

namespace utils {

class LogUtils {
public:
    // 日志级别枚举
    enum class LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };
    
    // 设置日志级别
    // 参数：level - 要设置的日志级别
    // 功能：控制哪些级别的日志会被输出
    static void setLogLevel(LogLevel level);
    
    // 输出调试日志
    // 参数：message - 要输出的日志消息
    static void debug(const std::string& message);
    
    // 输出普通信息日志
    // 参数：message - 要输出的日志消息
    static void info(const std::string& message);
    
    // 输出警告日志
    // 参数：message - 要输出的日志消息
    static void warning(const std::string& message);
    
    // 输出错误日志
    // 参数：message - 要输出的日志消息
    static void error(const std::string& message);
    
    // 输出带有格式的调试日志
    // 参数：format - 格式字符串
    //       ... - 可变参数列表
    static void debugFormat(const char* format, ...);
    
    // 输出带有格式的普通信息日志
    // 参数：format - 格式字符串
    //       ... - 可变参数列表
    static void infoFormat(const char* format, ...);
    
    // 输出带有格式的警告日志
    // 参数：format - 格式字符串
    //       ... - 可变参数列表
    static void warningFormat(const char* format, ...);
    
    // 输出带有格式的错误日志
    // 参数：format - 格式字符串
    //       ... - 可变参数列表
    static void errorFormat(const char* format, ...);
    
private:
    // 私有构造函数，防止实例化
    LogUtils();
    
    // 实际输出日志的函数
    // 参数：level - 日志级别
    //       message - 要输出的日志消息
    static void log(LogLevel level, const std::string& message);
    
    // 将日志级别转换为字符串
    // 参数：level - 日志级别
    // 返回：对应的字符串表示
    static std::string levelToString(LogLevel level);
    
    // 当前日志级别
    static LogLevel _currentLevel;
};

} // namespace utils

#endif // __LOG_UTILS_H__