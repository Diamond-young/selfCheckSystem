#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <string>

// HTTP响应头
const std::string HTTP_OK_HEADER = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: application/json\r\n"
    "Access-Control-Allow-Origin: *\r\n"
    "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
    "Access-Control-Allow-Headers: Content-Type\r\n"
    "Connection: close\r\n\r\n";

const std::string HTTP_ERROR_HEADER = 
    "HTTP/1.1 500 Internal Server Error\r\n"
    "Content-Type: application/json\r\n"
    "Access-Control-Allow-Origin: *\r\n"
    "Connection: close\r\n\r\n";

const std::string HTTP_NOT_FOUND = 
    "HTTP/1.1 404 Not Found\r\n"
    "Content-Type: application/json\r\n"
    "Access-Control-Allow-Origin: *\r\n"
    "Connection: close\r\n\r\n";

// 系统检查结果结构
struct SystemCheckResult {
    std::string status;   // "success", "warning", "error"
    std::string message;  // 详细描述信息
};

#endif // COMMON_TYPES_H