#include "request_handler.h"
#include "image_handler.h"
#include "../system/system_check.h"
#include "../config/config_manager.h"
#include "../utils/logger.h"
#include <sstream>
#include <algorithm>
#include <sys/socket.h>
#include <cstring> 
#include <unistd.h>
#include <nlohmann/json.hpp>
#include <arpa/inet.h>
#include <map>

using json = nlohmann::json;

// 定义 HTTP 响应头
const std::string RequestHandler::HTTP_OK_HEADER = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: application/json\r\n"
    "Access-Control-Allow-Origin: *\r\n"
    "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
    "Access-Control-Allow-Headers: Content-Type, Authorization\r\n"
    "Connection: close\r\n\r\n";

// 添加 OPTIONS 响应定义
const std::string RequestHandler::HTTP_OPTIONS_RESPONSE = 
    "HTTP/1.1 204 No Content\r\n"
    "Access-Control-Allow-Origin: *\r\n"
    "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
    "Access-Control-Allow-Headers: Content-Type, Authorization\r\n"
    "Access-Control-Max-Age: 86400\r\n"
    "Content-Length: 0\r\n"
    "Connection: close\r\n\r\n";



void RequestHandler::handle_connection(int client_socket) {
    char buffer[8192] = {0};
    ssize_t bytes_read = ::recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_read <= 0) {
        if (bytes_read == 0) {
            Logger::debug("客户端关闭连接");
        } else {
            Logger::error("接收错误: " + std::string(strerror(errno)));
        }
        ::close(client_socket);
        return;
    }
    
    buffer[bytes_read] = '\0';
    std::string request(buffer);
    
    // 获取客户端IP地址（调试用）
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    if (getpeername(client_socket, (struct sockaddr*)&client_addr, &addr_len) == 0) {
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), ip_str, INET_ADDRSTRLEN);
        Logger::info("来自 " + std::string(ip_str) + " 的请求");
    }
    
    // 记录请求（只记录前100字符）
    Logger::debug("收到请求: " + request.substr(0, std::min<size_t>(100, request.size())) + "...");
    
    process_request(client_socket, request);
    ::close(client_socket);
}

void RequestHandler::process_request(int client_socket, const std::string& request) {
    std::istringstream request_stream(request);
    std::string method, path, protocol;
    request_stream >> method >> path >> protocol;
    
    // 转换为大写方便比较
    std::string method_upper = method;
    std::transform(method_upper.begin(), method_upper.end(), method_upper.begin(), ::toupper);
    
    // 处理OPTIONS请求
    if (method_upper == "OPTIONS") {
        handle_options_request(client_socket);
        return;
    }
    
    // 提取请求体
    std::string body;
    size_t body_start = request.find("\r\n\r\n");
    if (body_start != std::string::npos && body_start + 4 < request.size()) {
        body = request.substr(body_start + 4);
    }
    
    // 处理图片请求
    if (path.find("/api/v1/image") == 0) {
        ImageHandler::handle_request(client_socket, path);
        return;
    }
    
    // 处理其他请求
    if (method_upper == "GET") {
        handle_get_request(client_socket, path);
    } else if (method_upper == "POST") {
        handle_post_request(client_socket, path, body);
    } else {
        const char* response = "HTTP/1.1 405 Method Not Allowed\r\n"
                               "Content-Type: application/json\r\n"
                               "Access-Control-Allow-Origin: *\r\n"
                               "Connection: close\r\n\r\n"
                               "{\"error\":\"Unsupported HTTP method\"}";
        ssize_t sent = ::send(client_socket, response, strlen(response), 0);
        if (sent < 0) {
            Logger::error("发送响应失败: " + std::string(strerror(errno)));
        }
    }
}

void RequestHandler::handle_options_request(int client_socket) {
    ssize_t sent = ::send(client_socket, HTTP_OPTIONS_RESPONSE.c_str(), HTTP_OPTIONS_RESPONSE.size(), 0);
    if (sent < 0) {
        Logger::error("发送OPTIONS响应失败: " + std::string(strerror(errno)));
    }
}

void RequestHandler::handle_post_request(int client_socket, const std::string& path, const std::string& body) {
    // 检查是否为配置更新请求
    if (path.find("/api/v1/config/") == 0) {
        std::string config_name = path.substr(15); // 跳过 "/api/v1/config/"
        
        try {
            // 解析JSON主体
            json json_body = json::parse(body);
            
            // 将JSON转换为key-value映射
            std::map<std::string, std::string> updates;
            for (auto& [key, value] : json_body.items()) {
                if (value.is_string()) {
                    updates[key] = value.get<std::string>();
                } else {
                    updates[key] = value.dump();
                }
            }
            
            // 保存配置
            SystemCheckResult result = ConfigManager::save_config(config_name, updates);
            
            // 构建响应
            std::string json_response = SystemCheck::create_json_response(result);
            std::string response = HTTP_OK_HEADER + json_response;
            
            ssize_t sent = ::send(client_socket, response.c_str(), response.size(), 0);
            if (sent < 0) {
                Logger::error("发送POST响应失败: " + std::string(strerror(errno)));
            }
            return;
        } catch (const std::exception& e) {
            Logger::error("处理配置更新失败: " + std::string(e.what()));
        }
    }
    
    // 默认错误响应
    const char* response = "HTTP/1.1 400 Bad Request\r\n"
                           "Content-Type: application/json\r\n"
                           "Access-Control-Allow-Origin: *\r\n"
                           "Connection: close\r\n\r\n"
                           "{\"error\":\"Invalid request\"}";
    ssize_t sent = ::send(client_socket, response, strlen(response), 0);
    if (sent < 0) {
        Logger::error("发送错误响应失败: " + std::string(strerror(errno)));
    }
}

void RequestHandler::handle_get_request(int client_socket, const std::string& path) {
    // 配置文件列表
    if (path == "/api/v1/config/files") {
        std::vector<std::string> files = ConfigManager::get_config_files();
        json response_json = {
            {"success", true},
            {"files", files}
        };
        
        std::string response = HTTP_OK_HEADER + response_json.dump();
        ssize_t sent = ::send(client_socket, response.c_str(), response.size(), 0);
        if (sent < 0) {
            Logger::error("发送配置列表失败: " + std::string(strerror(errno)));
        }
        return;
    }
    
    // 获取特定配置
    if (path.find("/api/v1/config/") == 0) {
        std::string config_name = path.substr(15); // 跳过 "/api/v1/config/"
        
        // 检查是否请求JSON格式
        bool json_format = false;
        if (size_t pos = config_name.find("/json"); pos != std::string::npos) {
            config_name = config_name.substr(0, pos);
            json_format = true;
        }
        
        std::string config_content;
        if (json_format) {
            config_content = ConfigManager::get_structured_config(config_name);
        } else {
            config_content = ConfigManager::get_current_config(config_name);
        }
        
        std::string response = HTTP_OK_HEADER + config_content;
        ssize_t sent = ::send(client_socket, response.c_str(), response.size(), 0);
        if (sent < 0) {
            Logger::error("发送配置内容失败: " + std::string(strerror(errno)));
        }
        return;
    }
    
    // 系统检查端点
    SystemCheckResult result;
    if (path == "/api/v1/check/ssh") {
        result = SystemCheck::check_ssh_connection();
    } else if (path == "/api/v1/check/memory") {
        result = SystemCheck::check_memory_usage();
    } else if (path == "/api/v1/check/gpu") {
        result = SystemCheck::check_gpu_frequency();
    } else if (path == "/api/v1/check/serial") {
        result = SystemCheck::check_serial_devices();
    } else if (path == "/api/v1/check/camera") {
        result = SystemCheck::check_camera_devices();
    } else if (path == "/") {
        const char* response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: application/json\r\n"
                               "Access-Control-Allow-Origin: *\r\n"
                               "Connection: close\r\n\r\n"
                               "{\"message\":\"Flight System Status API is running\"}";
        ssize_t sent = ::send(client_socket, response, strlen(response), 0);
        if (sent < 0) {
            Logger::error("发送欢迎响应失败: " + std::string(strerror(errno)));
        }
        return;
    } else {
        const char* response = "HTTP/1.1 404 Not Found\r\n"
                               "Content-Type: application/json\r\n"
                               "Access-Control-Allow-Origin: *\r\n"
                               "Connection: close\r\n\r\n"
                               "{\"error\":\"Endpoint not found\"}";
        ssize_t sent = ::send(client_socket, response, strlen(response), 0);
        if (sent < 0) {
            Logger::error("发送404响应失败: " + std::string(strerror(errno)));
        }
        return;
    }
    
    // 发送系统检查结果
    std::string json_response = SystemCheck::create_json_response(result);
    std::string response = HTTP_OK_HEADER + json_response;
    ssize_t sent = ::send(client_socket, response.c_str(), response.size(), 0);
    if (sent < 0) {
        Logger::error("发送系统检查结果失败: " + std::string(strerror(errno)));
    }
}