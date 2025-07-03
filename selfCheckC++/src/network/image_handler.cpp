#include "image_handler.h"
#include "../config/config_manager.h"
#include "../utils/file_utils.h"
#include "../utils/logger.h"
#include <sys/stat.h>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <sys/socket.h>  
#include <cstring>       
#include <unistd.h>      

namespace fs = std::filesystem;

void ImageHandler::handle_request(int client_socket, const std::string& path) {
    const std::string api_path = "/api/v1/image/";
    const size_t api_path_len = api_path.length();
    
    // 请求图片列表的特殊端点
    if (path == "/api/v1/image" || path == "/api/v1/image/") {
        send_image_list_response(client_socket);
        return;
    }
    
    // 提取文件名
    size_t pos = path.find(api_path);
    if (pos == std::string::npos) {
        const char* response = "HTTP/1.1 404 Not Found\r\n"
                               "Content-Type: application/json\r\n"
                               "Access-Control-Allow-Origin: *\r\n"
                               "Connection: close\r\n\r\n"
                               "{\"error\":\"Invalid image request path\"}";
        send(client_socket, response, strlen(response), 0);
        return;
    }
    
    std::string filename = path.substr(pos + api_path_len);
    Logger::info("请求图片文件: " + filename);
    
    // 安全检测：防止路径遍历攻击
    if (filename.find("..") != std::string::npos || 
        filename.find('/') != std::string::npos) {
        const char* response = "HTTP/1.1 400 Bad Request\r\n"
                               "Content-Type: application/json\r\n"
                               "Access-Control-Allow-Origin: *\r\n"
                               "Connection: close\r\n\r\n"
                               "{\"error\":\"Invalid filename\"}";
        send(client_socket, response, strlen(response), 0);
        return;
    }
    
    // 构建完整图片路径
    std::string image_dir = ConfigManager::get_image_directory();
    if (!image_dir.empty() && image_dir.back() != '/') {
        image_dir += '/';
    }
    std::string image_path = image_dir + filename;
    
    // 检查文件是否存在
    if (!fs::exists(image_path) || fs::is_directory(image_path)) {
        const char* response = "HTTP/1.1 404 Not Found\r\n"
                               "Content-Type: application/json\r\n"
                               "Access-Control-Allow-Origin: *\r\n"
                               "Connection: close\r\n\r\n"
                               "{\"error\":\"Image not found\"}";
        send(client_socket, response, strlen(response), 0);
        return;
    }
    
    // 发送图片
    send_image_response(client_socket, image_path);
}

void ImageHandler::send_image_list_response(int client_socket) {
    std::vector<std::string> images = get_available_images();
    
    std::ostringstream oss;
    oss << "HTTP/1.1 200 OK\r\n"
        << "Content-Type: application/json\r\n"
        << "Access-Control-Allow-Origin: *\r\n"
        << "Connection: close\r\n\r\n"
        << "{\"images\":[";
    
    for (size_t i = 0; i < images.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "\"" << images[i] << "\"";
    }
    oss << "]}";
    
    std::string response = oss.str();
    send(client_socket, response.c_str(), response.size(), 0);
}

void ImageHandler::send_image_response(int client_socket, const std::string& image_path) {
    // 获取文件大小
    struct stat file_stat;
    if (stat(image_path.c_str(), &file_stat) != 0) {
        Logger::error("获取文件大小失败: " + std::string(strerror(errno)));
        const char* response = "HTTP/1.1 500 Internal Server Error\r\n"
                               "Content-Type: application/json\r\n"
                               "Access-Control-Allow-Origin: *\r\n"
                               "Connection: close\r\n\r\n"
                               "{\"error\":\"Failed to get file size\"}";
        send(client_socket, response, strlen(response), 0);
        return;
    }
    size_t file_size = file_stat.st_size;
    
    // 确定内容类型
    std::string content_type = get_content_type(image_path);
    
    // 构建响应头
    std::ostringstream header_oss;
    header_oss << "HTTP/1.1 200 OK\r\n"
               << "Content-Type: " << content_type << "\r\n"
               << "Content-Length: " << file_size << "\r\n"
               << "Access-Control-Allow-Origin: *\r\n"
               << "Connection: close\r\n\r\n";
    
    std::string header = header_oss.str();
    send(client_socket, header.c_str(), header.size(), 0);
    
    // 发送文件内容
    std::ifstream file(image_path, std::ios::binary);
    if (!file) {
        Logger::error("无法打开图片文件: " + image_path);
        return;
    }
    
    const size_t BUFFER_SIZE = 65536;
    char buffer[BUFFER_SIZE];
    
    while (file) {
        file.read(buffer, BUFFER_SIZE);
        std::streamsize bytes_read = file.gcount();
        
        if (bytes_read > 0) {
            ssize_t bytes_sent = send(client_socket, buffer, bytes_read, 0);
            if (bytes_sent < 0) {
                Logger::error("发送图片数据中断: " + std::string(strerror(errno)));
                break;
            }
        }
    }
    
    Logger::info("图片发送完成: " + image_path);
}

std::vector<std::string> ImageHandler::get_available_images() {
    std::string image_dir = ConfigManager::get_image_directory();
    if (!image_dir.empty() && image_dir.back() != '/') {
        image_dir += '/';
    }
    return FileUtils::get_files_in_directory(image_dir);
}

std::string ImageHandler::get_content_type(const std::string& file_path) {
    fs::path path(file_path);
    std::string extension = path.extension().string();
    
    // 转换为小写
    std::transform(extension.begin(), extension.end(), extension.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    
    if (extension == ".jpg" || extension == ".jpeg") return "image/jpeg";
    if (extension == ".png") return "image/png";
    if (extension == ".gif") return "image/gif";
    if (extension == ".bmp") return "image/bmp";
    if (extension == ".webp") return "image/webp";
    
    return "application/octet-stream";
}