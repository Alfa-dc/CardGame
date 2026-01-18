// FileUtils.cpp
// 文件工具类实现
//
// 这个文件实现了FileUtils类的所有方法，提供跨平台的文件操作功能
// 基于Cocos2d-x的FileUtils，确保兼容性
//
#include "FileUtils.h"
#include <fstream>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include "cocos2d.h" // 使用Cocos2d-x的FileUtils

namespace utils {

// 构造函数
FileUtils::FileUtils() {
}

// 检查文件是否存在
bool FileUtils::fileExists(const std::string& filePath) {
    return cocos2d::FileUtils::getInstance()->isFileExist(filePath);
}

// 检查目录是否存在
bool FileUtils::directoryExists(const std::string& dirPath) {
    // Cocos2d-x的FileUtils没有直接检查目录是否存在的方法
    // 我们可以尝试列出目录内容，如果成功则说明目录存在
    std::vector<std::string> files = cocos2d::FileUtils::getInstance()->listFiles(dirPath);
    return !files.empty() || fileExists(dirPath);
}

// 创建目录
bool FileUtils::createDirectory(const std::string& dirPath) {
    return cocos2d::FileUtils::getInstance()->createDirectory(dirPath);
}

// 删除目录
bool FileUtils::removeDirectory(const std::string& dirPath) {
    // Cocos2d-x的FileUtils没有直接删除目录的方法
    // 这里使用标准库的remove_all，或者我们可以手动遍历删除
    #ifdef _WIN32
        std::string cmd = "rmdir /s /q \"" + dirPath + "\"";
        return system(cmd.c_str()) == 0;
    #else
        std::string cmd = "rm -rf \"" + dirPath + "\"";
        return system(cmd.c_str()) == 0;
    #endif
}

// 读取文本文件内容
std::string FileUtils::readTextFile(const std::string& filePath) {
    return cocos2d::FileUtils::getInstance()->getStringFromFile(filePath);
}

// 写入文本文件内容
bool FileUtils::writeTextFile(const std::string& filePath, const std::string& content, bool append) {
    FILE* fp = nullptr;
    if (append) {
        fp = fopen(filePath.c_str(), "ab");
    } else {
        fp = fopen(filePath.c_str(), "wb");
    }
    
    if (!fp) {
        return false;
    }
    
    size_t written = fwrite(content.c_str(), sizeof(char), content.size(), fp);
    fclose(fp);
    return written == content.size();
}

// 读取二进制文件内容
std::vector<char> FileUtils::readBinaryFile(const std::string& filePath) {
    cocos2d::Data data = cocos2d::FileUtils::getInstance()->getDataFromFile(filePath);
    return std::vector<char>(data.getBytes(), data.getBytes() + data.getSize());
}

// 写入二进制文件内容
bool FileUtils::writeBinaryFile(const std::string& filePath, const std::vector<char>& data, bool append) {
    FILE* fp = nullptr;
    if (append) {
        fp = fopen(filePath.c_str(), "ab");
    } else {
        fp = fopen(filePath.c_str(), "wb");
    }
    
    if (!fp) {
        return false;
    }
    
    size_t written = fwrite(data.data(), sizeof(char), data.size(), fp);
    fclose(fp);
    return written == data.size();
}

// 获取文件大小
long long FileUtils::getFileSize(const std::string& filePath) {
    cocos2d::Data data = cocos2d::FileUtils::getInstance()->getDataFromFile(filePath);
    return data.getSize();
}

// 获取文件扩展名
std::string FileUtils::getFileExtension(const std::string& filePath) {
    size_t dotPos = filePath.find_last_of(".");
    if (dotPos != std::string::npos && dotPos < filePath.size() - 1) {
        return filePath.substr(dotPos + 1);
    }
    return "";
}

// 获取文件名（不含路径）
std::string FileUtils::getFileName(const std::string& filePath) {
    size_t slashPos = filePath.find_last_of("/\\");
    if (slashPos != std::string::npos) {
        return filePath.substr(slashPos + 1);
    }
    return filePath;
}

// 获取文件名（不含扩展名）
std::string FileUtils::getFileNameWithoutExtension(const std::string& filePath) {
    std::string fileName = getFileName(filePath);
    size_t dotPos = fileName.find_last_of(".");
    if (dotPos != std::string::npos) {
        return fileName.substr(0, dotPos);
    }
    return fileName;
}

// 获取文件所在目录
std::string FileUtils::getDirectoryPath(const std::string& filePath) {
    size_t slashPos = filePath.find_last_of("/\\");
    if (slashPos != std::string::npos) {
        return filePath.substr(0, slashPos + 1);
    }
    return "";
}

// 拼接路径
std::string FileUtils::combinePaths(const std::string& dirPath, const std::string& fileName) {
    std::string result = dirPath;
    
    // 确保目录路径以分隔符结尾
    if (!result.empty() && result.back() != '/' && result.back() != '\\') {
        result += '/';
    }
    
    return result + fileName;
}

// 获取当前工作目录
std::string FileUtils::getCurrentWorkingDirectory() {
    return cocos2d::FileUtils::getInstance()->getWritablePath();
}

// 设置当前工作目录
bool FileUtils::setCurrentWorkingDirectory(const std::string& dirPath) {
    // Cocos2d-x的FileUtils没有直接设置当前工作目录的方法
    // 这里我们可以尝试使用系统命令
    #ifdef _WIN32
        std::string cmd = "cd \"" + dirPath + "\"";
        return system(cmd.c_str()) == 0;
    #else
        std::string cmd = "cd \"" + dirPath + "\"";
        return system(cmd.c_str()) == 0;
    #endif
}

// 获取绝对路径
std::string FileUtils::getAbsolutePath(const std::string& relativePath) {
    return cocos2d::FileUtils::getInstance()->fullPathForFilename(relativePath);
}

// 列出目录下的文件
std::vector<std::string> FileUtils::listFiles(const std::string& dirPath) {
    std::vector<std::string> allFiles = cocos2d::FileUtils::getInstance()->listFiles(dirPath);
    std::vector<std::string> files;
    
    for (const auto& file : allFiles) {
        if (fileExists(file)) {
            files.push_back(file);
        }
    }
    
    return files;
}

// 列出目录下的子目录
std::vector<std::string> FileUtils::listDirectories(const std::string& dirPath) {
    std::vector<std::string> allFiles = cocos2d::FileUtils::getInstance()->listFiles(dirPath);
    std::vector<std::string> dirs;
    
    for (const auto& file : allFiles) {
        if (directoryExists(file) && !fileExists(file)) {
            dirs.push_back(file);
        }
    }
    
    return dirs;
}

// 规范化路径分隔符
std::string FileUtils::normalizePath(const std::string& path) {
    std::string result = path;
    std::replace(result.begin(), result.end(), '\\', '/');
    return result;
}

// 平台相关的文件路径分隔符
const char* FileUtils::getPathSeparator() {
    return "/";
}

} // namespace utils