#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

class ConfigParser {
public:
    // 将YAML节点转换为JSON
    static nlohmann::json yaml_to_json(const YAML::Node& node);
    
    // 将JSON转换为YAML节点
    static YAML::Node json_to_yaml(const nlohmann::json& j);
};

#endif // CONFIG_PARSER_H