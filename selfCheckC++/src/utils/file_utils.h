#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <vector>
#include <map>
#include <filesystem>

namespace fs = std::filesystem;

class FileUtils {
public:
    // 确保目录存在
    static void ensure_directory_exists(const std::string& path);
    
    // 获取目录中的所有文件
    static std::vector<std::string> get_files_in_directory(const std::string& path);
    
    // 读取文件内容
    static std::string read_file(const std::string& path);
    
    // 写入文件内容
    static bool write_file(const std::string& path, const std::string& content);
    
    // 更新配置文件（保留注释）
    static void update_config_file(const std::string& file_path, 
                                  const std::map<std::string, std::string>& updates);
};

#endif // FILE_UTILS_H