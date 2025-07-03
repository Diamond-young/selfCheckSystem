#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include "common_types.h"
#include "utils/file_utils.h"
#include "config_parser.h"
#include <unordered_map>
#include <map>

class ConfigManager {
    public:
        // 初始化配置系统
        static void initialize();
        
        // 加载所有配置
        static void load_all_configs();
        
        // 加载特定配置
        static void load_config(const std::string& config_name);
        
        // 保存配置（返回操作结果）
        static SystemCheckResult save_config(const std::string& config_name, 
                                            const std::map<std::string, std::string>& updates);
        
        // 解析主配置文件中的特定设置
        static void parse_main_config();
        
        // 移除端口获取方法
        // 不再需要 static int get_port();
        
        // 获取当前配置文件内容
        static std::string get_current_config(const std::string& config_name);
        
        // 获取结构化配置（JSON格式）
        static std::string get_structured_config(const std::string& config_name);
        
        // 获取配置文件列表
        static std::vector<std::string> get_config_files();
        
        // 获取图片目录（从配置文件中）
        static std::string get_image_directory();
    
    private:
        // 配置文件路径映射
        static const std::unordered_map<std::string, std::string> CONFIG_PATHS;
        
        // 配置存储
        static std::unordered_map<std::string, std::string> config_store;
    };
    
    #endif // CONFIG_MANAGER_H