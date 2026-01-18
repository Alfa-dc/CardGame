// FileUtils.h
// 文件工具类 - 提供文件操作相关的辅助功能
//
// 这个类提供了各种文件操作的辅助方法，包括：
// 1. 检查文件是否存在
// 2. 读取文件内容
// 3. 写入文件内容
// 4. 获取文件大小
// 5. 获取文件路径相关信息
// 6. 创建和删除目录
//
// 核心功能：
// 1. 封装了C++标准库的文件操作，提供更简洁的接口
// 2. 支持跨平台的文件路径处理
// 3. 提供了错误处理机制
// 4. 支持文本文件和二进制文件的读写
//
// 使用示例：
// if (FileUtils::fileExists("config.json")) {
//     std::string content = FileUtils::readTextFile("config.json");
//     // 处理文件内容
// }
//
#ifndef __FILE_UTILS_H__
#define __FILE_UTILS_H__

#include <string>
#include <vector>

namespace utils {

class FileUtils {
public:
    // 检查文件是否存在
    // 参数：filePath - 要检查的文件路径
    // 返回：文件存在返回true，否则返回false
    static bool fileExists(const std::string& filePath);
    
    // 检查目录是否存在
    // 参数：dirPath - 要检查的目录路径
    // 返回：目录存在返回true，否则返回false
    static bool directoryExists(const std::string& dirPath);
    
    // 创建目录
    // 参数：dirPath - 要创建的目录路径
    // 返回：创建成功返回true，否则返回false
    static bool createDirectory(const std::string& dirPath);
    
    // 删除目录
    // 参数：dirPath - 要删除的目录路径
    // 返回：删除成功返回true，否则返回false
    static bool removeDirectory(const std::string& dirPath);
    
    // 读取文本文件内容
    // 参数：filePath - 要读取的文件路径
    // 返回：文件内容字符串
    // 注意：如果文件不存在或读取失败，返回空字符串
    static std::string readTextFile(const std::string& filePath);
    
    // 写入文本文件内容
    // 参数：
    //   filePath - 要写入的文件路径
    //   content - 要写入的内容
    //   append - 是否追加到文件末尾，默认覆盖
    // 返回：写入成功返回true，否则返回false
    static bool writeTextFile(const std::string& filePath, const std::string& content, bool append = false);
    
    // 读取二进制文件内容
    // 参数：filePath - 要读取的文件路径
    // 返回：二进制数据向量
    // 注意：如果文件不存在或读取失败，返回空向量
    static std::vector<char> readBinaryFile(const std::string& filePath);
    
    // 写入二进制文件内容
    // 参数：
    //   filePath - 要写入的文件路径
    //   data - 要写入的二进制数据
    //   append - 是否追加到文件末尾，默认覆盖
    // 返回：写入成功返回true，否则返回false
    static bool writeBinaryFile(const std::string& filePath, const std::vector<char>& data, bool append = false);
    
    // 获取文件大小
    // 参数：filePath - 要获取大小的文件路径
    // 返回：文件大小（字节），失败返回-1
    static long long getFileSize(const std::string& filePath);
    
    // 获取文件扩展名
    // 参数：filePath - 文件路径
    // 返回：文件扩展名（不含点），如"txt"、"json"
    static std::string getFileExtension(const std::string& filePath);
    
    // 获取文件名（不含路径）
    // 参数：filePath - 文件路径
    // 返回：文件名，如"config.json"
    static std::string getFileName(const std::string& filePath);
    
    // 获取文件名（不含扩展名）
    // 参数：filePath - 文件路径
    // 返回：文件名（不含扩展名），如"config"
    static std::string getFileNameWithoutExtension(const std::string& filePath);
    
    // 获取文件所在目录
    // 参数：filePath - 文件路径
    // 返回：文件所在目录路径
    static std::string getDirectoryPath(const std::string& filePath);
    
    // 拼接路径
    // 参数：
    //   dirPath - 目录路径
    //   fileName - 文件名
    // 返回：拼接后的完整路径
    static std::string combinePaths(const std::string& dirPath, const std::string& fileName);
    
    // 获取当前工作目录
    // 返回：当前工作目录路径
    static std::string getCurrentWorkingDirectory();
    
    // 设置当前工作目录
    // 参数：dirPath - 要设置的工作目录
    // 返回：设置成功返回true，否则返回false
    static bool setCurrentWorkingDirectory(const std::string& dirPath);
    
    // 获取绝对路径
    // 参数：relativePath - 相对路径
    // 返回：绝对路径
    static std::string getAbsolutePath(const std::string& relativePath);
    
    // 列出目录下的文件
    // 参数：dirPath - 目录路径
    // 返回：目录下的文件列表
    static std::vector<std::string> listFiles(const std::string& dirPath);
    
    // 列出目录下的子目录
    // 参数：dirPath - 目录路径
    // 返回：目录下的子目录列表
    static std::vector<std::string> listDirectories(const std::string& dirPath);
    
private:
    // 私有构造函数，防止实例化
    FileUtils();
    
    // 规范化路径分隔符
    // 参数：path - 要规范化的路径
    // 返回：规范化后的路径
    static std::string normalizePath(const std::string& path);
    
    // 平台相关的文件路径分隔符
    static const char* getPathSeparator();
};

} // namespace utils

#endif // __FILE_UTILS_H__