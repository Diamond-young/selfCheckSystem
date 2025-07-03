#include "file_utils.h"
#include "logger.h"
#include <fstream>
#include <cctype>
#include <algorithm>
#include <set>
#include <sstream>
#include <iomanip>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

void FileUtils::ensure_directory_exists(const std::string& path) {
    try {
        fs::path dir_path(path);
        if (!fs::exists(dir_path)) {
            if (fs::create_directories(dir_path)) {
                Logger::info("已创建目录: " + path);
            } else {
                Logger::warning("无法创建目录: " + path);
            }
        } else {
            Logger::debug("目录已存在: " + path);
        }
    } catch (const std::exception& e) {
        Logger::error("创建目录失败: " + path + " - " + e.what());
    }
}

std::vector<std::string> FileUtils::get_files_in_directory(const std::string& path) {
    std::vector<std::string> files;
    try {
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().filename().string());
            }
        }
    } catch (const std::exception& e) {
        Logger::error("获取目录文件失败: " + path + " - " + e.what());
    }
    return files;
}

std::string FileUtils::read_file(const std::string& path) {
    try {
        std::ifstream file(path);
        if (!file) {
            throw std::runtime_error("无法打开文件: " + path);
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), 
                            std::istreambuf_iterator<char>());
        return content;
    } catch (const std::exception& e) {
        Logger::error("读取文件失败: " + path + " - " + e.what());
        return "";
    }
}

bool FileUtils::write_file(const std::string& path, const std::string& content) {
    try {
        std::ofstream file(path);
        if (!file) {
            throw std::runtime_error("无法打开文件进行写入: " + path);
        }
        
        file << content;
        return true;
    } catch (const std::exception& e) {
        Logger::error("写入文件失败: " + path + " - " + e.what());
        return false;
    }
}

// 辅助函数：判断值是否需要引号
bool needs_quotes(const std::string& value) {
    // 如果值包含特殊字符、空格或以数字开头，则需要引号
    return value.find_first_of(":{}[],'\"\\|>&%") != std::string::npos ||
           value.find(' ') != std::string::npos ||
           (!value.empty() && (isdigit(value[0]) || value[0] == '.'));
}

// 辅助函数：格式化浮点数
std::string format_float(double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(10) << value;
    std::string str = oss.str();
    
    // 移除不必要的尾部零和小数点
    size_t pos = str.find_last_not_of('0');
    if (pos != std::string::npos && str[pos] == '.') {
        pos--;
    }
    return str.substr(0, pos + 1);
}

// 递归更新配置函数
void update_nested_config(std::vector<std::string>& lines, 
                          const std::string& full_key,
                          const std::string& value,
                          std::set<size_t>& updated_lines) {
    // 查找要更新的键
    for (size_t i = 0; i < lines.size(); i++) {
        if (updated_lines.find(i) != updated_lines.end()) continue;
        
        const std::string& line = lines[i];
        
        // 计算缩进
        size_t indent_len = 0;
        while (indent_len < line.size() && (line[indent_len] == ' ' || line[indent_len] == '\t')) {
            indent_len++;
        }
        
        // 跳过注释行和空行
        if (indent_len >= line.size() || line[indent_len] == '#') {
            continue;
        }
        
        // 查找键
        size_t colon_pos = line.find(':', indent_len);
        if (colon_pos == std::string::npos) {
            continue;
        }
        
        // 提取键
        std::string key = line.substr(indent_len, colon_pos - indent_len);
        // 清理键名
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        key.erase(std::remove(key.begin(), key.end(), '\"'), key.end());
        
        // 检查是否是我们要找的键
        size_t dot_pos = full_key.find('.');
        std::string current_key = (dot_pos == std::string::npos) ? full_key : full_key.substr(0, dot_pos);
        
        if (key == current_key) {
            // 如果这是叶子节点
            if (dot_pos == std::string::npos) {
                // 定位值位置
                size_t value_start = colon_pos + 1;
                while (value_start < line.size() && (line[value_start] == ' ' || line[value_start] == '\t')) {
                    value_start++;
                }
                
                // 保留注释部分
                size_t comment_start = line.find('#', value_start);
                std::string comment = (comment_start != std::string::npos) ? 
                    line.substr(comment_start) : "";
                
                // 构建新行
                std::string new_line = line.substr(0, indent_len) + 
                                      key + 
                                      ": " + value;
                
                if (!comment.empty()) {
                    new_line += " " + comment;
                }
                
                lines[i] = new_line;
                updated_lines.insert(i);
                return;
            }
            // 否则递归处理子键
            else {
                std::string sub_key = full_key.substr(dot_pos + 1);
                update_nested_config(lines, sub_key, value, updated_lines);
                return;
            }
        }
    }
    
    // 如果没找到，作为新键添加
    Logger::warning("未找到配置项: " + full_key);
}

void FileUtils::update_config_file(const std::string& file_path, 
                                   const std::map<std::string, std::string>& updates) {
    // 读取原始文件内容
    std::vector<std::string> lines;
    {
        std::ifstream in_file(file_path);
        if (!in_file.is_open()) {
            Logger::error("无法打开配置文件: " + file_path);
            throw std::runtime_error("无法打开配置文件");
        }

        std::string line;
        while (std::getline(in_file, line)) {
            lines.push_back(line);
        }
    }

    // 跟踪已更新的行
    std::set<size_t> updated_lines;
    
    // 处理每个更新项
    for (const auto& [key, value] : updates) {
        // 检查值是否是JSON对象
        if (value.front() == '{' && value.back() == '}') {
            try {
                // 解析JSON对象
                auto j = json::parse(value);
                
                // 对于JSON对象中的每个键值对
                for (auto& [sub_key, sub_val] : j.items()) {
                    std::string full_path = key + "." + sub_key;
                    std::string val_str = sub_val.is_string() ? 
                        sub_val.get<std::string>() : sub_val.dump();
                    
                    // 递归更新配置
                    update_nested_config(lines, full_path, val_str, updated_lines);
                }
                continue;
            } catch (...) {
                // 不是有效的JSON，继续正常处理
            }
        }
        
        // 处理单个值更新
        update_nested_config(lines, key, value, updated_lines);
    }
    
    // 写回文件
    std::ofstream out_file(file_path);
    if (out_file.is_open()) {
        for (const auto& line : lines) {
            out_file << line << "\n";
        }
        Logger::info("配置文件更新完成: " + file_path);
    } else {
        Logger::error("无法写入配置文件: " + file_path);
        throw std::runtime_error("无法写入配置文件");
    }
}