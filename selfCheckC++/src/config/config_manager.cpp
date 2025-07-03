#include "config_manager.h"
#include "config_parser.h"
#include "logger.h"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

// 静态成员初始化
const std::unordered_map<std::string, std::string> ConfigManager::CONFIG_PATHS = {
    {"main","../src/config/config.yaml"},
    {"trans", "../src/config/trans_config.yaml"},
    {"obj_trans", "../src/config/obj_loc_config.yaml"},
    {"DMatcher_plane","../src/config/DMatcher_plane.yaml"},
    {"DMatcher_target","../src/config/DMatcher_target.yaml"}
};

std::unordered_map<std::string, std::string> ConfigManager::config_store;

void ConfigManager::initialize() {
    load_all_configs();
}

void ConfigManager::load_all_configs() {
    for (const auto& [name, path] : CONFIG_PATHS) {
        load_config(name);
    }
}

void ConfigManager::load_config(const std::string& config_name) {
    auto it = CONFIG_PATHS.find(config_name);
    if (it == CONFIG_PATHS.end()) {
        Logger::warning("找不到配置文件: " + config_name);
        return;
    }
    
    const std::string& file_path = it->second;
    try {
        // 确保配置文件存在
        if (!fs::exists(file_path)) {
            Logger::warning("配置文件不存在，创建默认配置: " + file_path);
            FileUtils::write_file(file_path, "# " + config_name + " 默认配置\n");
        }
        
        // 读取文件内容
        std::string content = FileUtils::read_file(file_path);
        config_store[config_name] = content;
        
        Logger::info("成功加载配置: " + config_name + " (" + file_path + ")");
        
        // 如果是主配置文件，解析应用全局设置
        if (config_name == "main") {
            parse_main_config();
        }
    } catch (const std::exception& e) {
        Logger::error("加载配置文件失败: " + config_name + " - " + e.what());
    }
}

SystemCheckResult ConfigManager::save_config(const std::string& config_name, 
                                            const std::map<std::string, std::string>& updates) {
    auto it = CONFIG_PATHS.find(config_name);
    if (it == CONFIG_PATHS.end()) {
        return {"error", "不支持的配置文件: " + config_name};
    }
    
    const std::string& file_path = it->second;
    Logger::info("保存配置文件: " + config_name + " (" + file_path + ")");
    
    try {
        // 更新配置文件
        FileUtils::update_config_file(file_path, updates);
        
        // 重新加载配置文件
        load_config(config_name);
        
        return {"success", "配置保存成功"};
    } catch (const std::exception& e) {
        std::string error_msg = "保存配置文件失败: " + std::string(e.what());
        Logger::error(error_msg);
        return {"error", error_msg};
    }
}

void ConfigManager::parse_main_config() {
    try {
        auto yaml = YAML::Load(config_store["main"]);
        if (yaml["system"] && yaml["system"]["image_directory"]) {
            std::string image_dir = yaml["system"]["image_directory"].as<std::string>();
            
            // 确保目录存在
            FileUtils::ensure_directory_exists(image_dir);
            Logger::info("设置图片目录: " + image_dir);
        }
    } catch (const std::exception& e) {
        Logger::error("解析主配置文件失败: " + std::string(e.what()));
    }
}

std::string ConfigManager::get_current_config(const std::string& config_name) {
    auto it = config_store.find(config_name);
    if (it != config_store.end()) {
        return it->second;
    }
    return "# 默认空配置\n";
}

std::string ConfigManager::get_structured_config(const std::string& config_name) {
    try {
        auto it = config_store.find(config_name);
        if (it != config_store.end()) {
            auto yaml = YAML::Load(it->second);
            json j = ConfigParser::yaml_to_json(yaml);
            
            return json{
                {"success", true},
                {"config_name", config_name},
                {"config", j}
            }.dump(2);
        }
        
        return json{
            {"success", false},
            {"error", "Configuration not loaded: " + config_name}
        }.dump(2);
    } catch (const std::exception& e) {
        return json{
            {"success", false},
            {"error", "Failed to convert config: " + std::string(e.what())}
        }.dump(2);
    }
}

std::vector<std::string> ConfigManager::get_config_files() {
    std::vector<std::string> files;
    for (const auto& pair : CONFIG_PATHS) {
        files.push_back(pair.first);
    }
    return files;
}


std::string ConfigManager::get_image_directory() {
    try {
        auto yaml = YAML::Load(config_store["main"]);
        if (yaml["system"] && yaml["system"]["image_directory"]) {
            return yaml["system"]["image_directory"].as<std::string>();
        }
    } catch (...) {
        // 忽略错误
    }
    return "images/"; // 默认目录
}