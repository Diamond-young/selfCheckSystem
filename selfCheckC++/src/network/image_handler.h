#ifndef IMAGE_HANDLER_H
#define IMAGE_HANDLER_H

#include <string>
#include <vector>

class ImageHandler {
public:
    // 处理图片请求
    static void handle_request(int client_socket, const std::string& path);
    
    // 获取所有可用图片
    static std::vector<std::string> get_available_images();

private:
    // 发送图片响应
    static void send_image_response(int client_socket, const std::string& image_path);
    
    // 发送图片列表响应
    static void send_image_list_response(int client_socket);
    
    // 确定内容类型
    static std::string get_content_type(const std::string& file_path);
};

#endif // IMAGE_HANDLER_H