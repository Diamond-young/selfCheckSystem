#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <cstring>
#include <arpa/inet.h>
#include "config/config_manager.h"
#include "network/request_handler.h"
#include "system/system_check.h"
#include "types/common_types.h"
#include "utils/logger.h"

using namespace std;

int main(int argc, char* argv[]) {
    // 初始化日志系统
    Logger::initialize(Logger::DEBUG);
    Logger::info("启动飞行系统状态检测服务器");
    
    // 初始化配置管理器
    ConfigManager::initialize();
    
    // 设置固定端口
    const int SERVER_PORT = 8080; // 使用固定端口 8080
    Logger::info("服务器端口: " + to_string(SERVER_PORT));
    
    // 创建socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        Logger::error("创建socket失败: " + string(strerror(errno)));
        return 1;
    }
    
    // 设置套接字选项允许地址重用
    int reuse = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        Logger::error("设置SO_REUSEADDR失败: " + string(strerror(errno)));
        close(server_socket);
        return 1;
    }
    
    // 配置服务器地址
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT); // 使用固定端口
    
    // 绑定socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        Logger::error("绑定地址失败: " + string(strerror(errno)));
        close(server_socket);
        return 1;
    }
    
    // 开始监听
    if (listen(server_socket, 10) < 0) {
        Logger::error("监听失败: " + string(strerror(errno)));
        close(server_socket);
        return 1;
    }
    
    // 获取本地IP地址
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(server_addr.sin_addr), ip_str, INET_ADDRSTRLEN);
    Logger::info("服务器已启动，IP: " + string(ip_str) + " 端口: " + to_string(SERVER_PORT));
    
    Logger::info("API列表:");
    Logger::info("  配置文件列表:  http://" + string(ip_str) + ":" + to_string(SERVER_PORT) + "/api/v1/config/files");
    Logger::info("  获取配置文件:  http://" + string(ip_str) + ":" + to_string(SERVER_PORT) + "/api/v1/config/{name}");
    Logger::info("  SSH状态:       http://" + string(ip_str) + ":" + to_string(SERVER_PORT) + "/api/v1/check/ssh");
    Logger::info("  内存状态:      http://" + string(ip_str) + ":" + to_string(SERVER_PORT) + "/api/v1/check/memory");
    Logger::info("  GPU频率:       http://" + string(ip_str) + ":" + to_string(SERVER_PORT) + "/api/v1/check/gpu");
    Logger::info("  串口设备状态:  http://" + string(ip_str) + ":" + to_string(SERVER_PORT) + "/api/v1/check/serial");
    Logger::info("  相机设备状态:  http://" + string(ip_str) + ":" + to_string(SERVER_PORT) + "/api/v1/check/camera");
    Logger::info("  获取图片列表:  http://" + string(ip_str) + ":" + to_string(SERVER_PORT) + "/api/v1/image");
    
    // 主循环
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        
        if (client_socket < 0) {
            Logger::error("接受连接失败: " + string(strerror(errno)));
            continue;
        }
        
        // 获取客户端IP地址
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        Logger::info("接受来自 " + string(client_ip) + " 的连接");
        
        // 创建线程处理连接
        thread(RequestHandler::handle_connection, client_socket).detach();
    }
    
    close(server_socket);
    return 0;
}