#include "config_parser.h"

using json = nlohmann::json;

json ConfigParser::yaml_to_json(const YAML::Node& node) {
    if (node.IsScalar()) {
        try {
            if (node.Tag() == "!!int") {
                return node.as<int>();
            } else if (node.Tag() == "!!float") {
                return node.as<double>();
            } else if (node.Tag() == "!!bool") {
                return node.as<bool>();
            }
            return node.as<std::string>();
        } catch (...) {
            return node.as<std::string>();
        }
    } else if (node.IsMap()) {
        json j = json::object();
        for (const auto& key_value : node) {
            std::string key = key_value.first.as<std::string>();
            j[key] = yaml_to_json(key_value.second);
        }
        return j;
    } else if (node.IsSequence()) {
        json j = json::array();
        for (const auto& item : node) {
            j.push_back(yaml_to_json(item));
        }
        return j;
    }
    return nullptr;
}

YAML::Node ConfigParser::json_to_yaml(const json& j) {
    if (j.is_object()) {
        YAML::Node node(YAML::NodeType::Map);
        for (auto& el : j.items()) {
            node[el.key()] = json_to_yaml(el.value());
        }
        return node;
    } else if (j.is_array()) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto& el : j) {
            node.push_back(json_to_yaml(el));
        }
        return node;
    } else if (j.is_string()) {
        return YAML::Node(j.get<std::string>());
    } else if (j.is_number_integer()) {
        return YAML::Node(j.get<int>());
    } else if (j.is_number_float()) {
        return YAML::Node(j.get<double>());
    } else if (j.is_boolean()) {
        return YAML::Node(j.get<bool>());
    }
    return YAML::Node();
}