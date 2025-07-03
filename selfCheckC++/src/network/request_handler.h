#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <string>

class RequestHandler {
public:
    static void handle_connection(int client_socket);
private:
    static void process_request(int client_socket, const std::string& request);
    static void handle_options_request(int client_socket);
    static void handle_post_request(int client_socket, const std::string& path, const std::string& body);
    static void handle_get_request(int client_socket, const std::string& path);
    
    // 添加 HTTP 响应头声明
    static const std::string HTTP_OK_HEADER;
    static const std::string HTTP_OPTIONS_RESPONSE;  // 添加这行
};

#endif // REQUEST_HANDLER_H