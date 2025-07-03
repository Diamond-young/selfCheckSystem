#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> 
#include <sys/sysinfo.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <filesystem>
#include <cstring>
#include <algorithm>
#include <mutex>
#include <thread>
#include <arpa/inet.h>
#include <vector>  
#include <cmath>   
#include <cctype>  
#include <dirent.h>

namespace fs = std::filesystem;

using namespace std;
// 图像存储路径 - 改为当前目录下的images文件夹
const string IMAGE_DIRECTORY = "images/";
// RK3588特定配置
const vector<string> GPU_TEMP_PATHS = {
    "/sys/class/thermal/thermal_zone0/temp",
    "/sys/class/thermal/thermal_zone1/temp"
};

const vector<string> GPU_FREQ_PATHS = {
    "/sys/devices/platform/fb000000.gpu/devfreq/fb000000.gpu/cur_freq",  // 主路径
    "/sys/class/devfreq/fb000000.gpu/cur_freq",  // 备选路径
    "/sys/class/misc/mali0/device/devfreq/cur_freq"  // 替代路径s
};

const vector<string> SERIAL_PORTS = {
    "/dev/ttyS0", "/dev/ttyS1", "/dev/ttyS2", 
    "/dev/ttyS3", "/dev/ttyS4", "/dev/ttyS5",
    "/dev/ttyS6", "/dev/ttyS7", "/dev/ttyS8"
};

// HTTP 响应头 
const string HTTP_OK_HEADER = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: application/json\r\n"
    "Access-Control-Allow-Origin: *\r\n"
    "Access-Control-Allow-Methods: GET, OPTIONS\r\n"
    "Access-Control-Allow-Headers: Content-Type\r\n"
    "Connection: close\r\n\r\n";

const string HTTP_ERROR_HEADER = 
    "HTTP/1.1 500 Internal Server Error\r\n"
    "Content-Type: application/json\r\n"
    "Access-Control-Allow-Origin: *\r\n"
    "Connection: close\r\n\r\n";

const string HTTP_NOT_FOUND = 
    "HTTP/1.1 404 Not Found\r\n"
    "Content-Type: application/json\r\n"
    "Access-Control-Allow-Origin: *\r\n"
    "Connection: close\r\n\r\n";

const string HTTP_NO_CONTENT = 
    "HTTP/1.1 204 No Content\r\n"
    "Access-Control-Allow-Origin: *\r\n"
    "Connection: close\r\n\r\n";


// 图像响应头
const string HTTP_IMAGE_HEADER = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: image/jpg\r\n" 
    "Access-Control-Allow-Origin: *\r\n"
    "Connection: close\r\n\r\n";
    
// 日志锁
mutex log_mutex;

// 状态检测结果结构体
struct SystemCheckResult {
    string status;
    string message;
};

// 日志函数
void log_message(const string& message) {
    lock_guard<mutex> lock(log_mutex);
    time_t t = time(nullptr);
    tm tm = *localtime(&t);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S] ", &tm);
    
    cout << buffer << message << endl;
}

// IP 地址手动转换函数
string ip_to_string(in_addr addr) {
    uint32_t ip_addr = ntohl(addr.s_addr);
    uint8_t b1 = (ip_addr >> 24) & 0xFF;
    uint8_t b2 = (ip_addr >> 16) & 0xFF;
    uint8_t b3 = (ip_addr >> 8) & 0xFF;
    uint8_t b4 = ip_addr & 0xFF;
    
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%d.%d.%d.%d", b1, b2, b3, b4);
    
    return string(buffer);
}

// SSH 连接检测
SystemCheckResult check_ssh_connection() {
    log_message("执行 SSH 连接检测");
    
    // 检测SSH服务状态
    int service_status = system("systemctl is-active --quiet ssh");
    if (service_status == 0) {
        // 检查端口监听
        int port_status = system("ss -tuln | grep -q ':22'");
        if (port_status == 0) {
            return {"success", "SSH服务运行正常，端口22已监听"};
        }
        return {"warning", "SSH服务已启动，但未检测到端口监听"};
    }
    
    return {"error", "SSH服务未运行"};
}

// 内存占用检测
SystemCheckResult check_memory_usage() {
    log_message("执行内存占用检测");
    
    ifstream meminfo_file("/proc/meminfo");
    if (!meminfo_file.is_open()) {
        return {"error", "无法打开/proc/meminfo"};
    }
    
    long total_ram = 0, free_ram = 0, buffers = 0, cached = 0;
    string line;
    
    while (getline(meminfo_file, line)) {
        if (line.find("MemTotal") != string::npos) {
            sscanf(line.c_str(), "MemTotal: %ld kB", &total_ram);
        } else if (line.find("MemFree") != string::npos) {
            sscanf(line.c_str(), "MemFree: %ld kB", &free_ram);
        } else if (line.find("Buffers") != string::npos) {
            sscanf(line.c_str(), "Buffers: %ld kB", &buffers);
        } else if (line.find("Cached") != string::npos) {
            sscanf(line.c_str(), "Cached: %ld kB", &cached);
        }
    }
    meminfo_file.close();
    
    if (total_ram <= 0) {
        return {"error", "无效的内存信息"};
    }
    
    long available_ram = free_ram + buffers + cached;
    long used_ram = total_ram - available_ram;
    int usage_percent = static_cast<int>((used_ram * 100) / total_ram);
    
    string status = "success";
    if (usage_percent > 85) {
        status = "error";
    } else if (usage_percent > 70) {
        status = "warning";
    }
    
    ostringstream oss;
    oss << "总内存: " << total_ram/1024 << "MB, 已用: " << used_ram/1024 
        << "MB, 可用: " << available_ram/1024 << "MB, 使用率: " << usage_percent << "%";
    
    return {status, oss.str()};
}

// RK3588 GPU 温度检测
int get_gpu_temperature() {
    for (const auto& temp_path : GPU_TEMP_PATHS) {
        try {
            if (fs::exists(temp_path)) {
                ifstream temp_file(temp_path);
                if (temp_file.is_open()) {
                    int temp_value;
                    temp_file >> temp_value;
                    temp_file.close();
                    
                    // RK3588温度传感器输出为毫摄氏度
                    if (temp_value > 10000) {
                        return temp_value / 1000; // 转换为摄氏度
                    }
                    return temp_value; // 已经是摄氏度
                }
            }
        } catch (...) {
            // 忽略错误继续尝试
        }
    }
    
    // 尝试通过命令行获取温度
    FILE* pipe = popen("cat /sys/class/thermal/thermal_zone*/temp | sort -nr | head -1", "r");
    if (pipe) {
        char buffer[16];
        if (fgets(buffer, sizeof(buffer), pipe)) {
            int max_temp = atoi(buffer);
            if (max_temp > 10000) return max_temp / 1000;
            if (max_temp > 100) return max_temp;
            return max_temp / 100;
        }
        pclose(pipe);
    }
    
    return -1; // 表示无法获取温度
}

// RK3588 GPU频点设置检测
SystemCheckResult check_gpu_frequency() {
    log_message("执行 RK3588 GPU频率检测");
    
    long current_freq = -1;
    string freq_path_used = "";

    // 按优先级尝试所有可能的路径
    for (const auto& path : GPU_FREQ_PATHS) {
        try {
            if (fs::exists(path)) {
                ifstream freq_file(path);
                if (freq_file.is_open()) {
                    string freq_str;
                    getline(freq_file, freq_str);
                    freq_file.close();
                    
                    // 确保读取的是有效数字
                    if (!freq_str.empty() && all_of(freq_str.begin(), freq_str.end(), ::isdigit)) {
                        current_freq = stol(freq_str);
                        freq_path_used = path;
                        break;
                    }
                }
            }
        } catch (const exception& e) {
            log_message(string("尝试访问 ") + path + " 时出错: " + e.what());
        }
    }
    
    // 如果都没有找到，尝试备用方案
    if (current_freq == -1) {
        // 尝试通过命令获取
        FILE* pipe = popen("cat /sys/class/devfreq/ff9a0000.gpu/cur_freq 2>/dev/null", "r");
        if (pipe) {
            char buffer[64];
            if (fgets(buffer, sizeof(buffer), pipe)) {
                current_freq = atol(buffer);
                freq_path_used = "命令行获取";
            }
            pclose(pipe);
        }
    }
    
    if (current_freq == -1) {
        return {"error", "无法获取GPU频率信息: 没有有效的访问路径"};
    }

    // 获取GPU温度
    int gpu_temp = get_gpu_temperature();
    
    // 根据RK3588文档，频率范围可能为100-800MHz或100-1000MHz
    const long MIN_FREQ = 100000000;  // 100 MHz
    const long MAX_FREQ = 900000000;  // 900 MHz
    
    string status = "success";
    string temp_info = "";
    
    if (gpu_temp > 0) {
        ostringstream oss;
        oss << ", 温度: " << gpu_temp << "°C";
        temp_info = oss.str();
        
        if (gpu_temp > 85) {
            status = "warning";
        }
    }
    
    ostringstream oss;
    oss << "当前GPU频率: " << (current_freq / 1000000) << " MHz (来源: " << freq_path_used << ")" << temp_info;
    
    // 检查频率是否在合理范围
    if (current_freq < MIN_FREQ || current_freq > MAX_FREQ) {
        status = "warning";
        oss << " (异常频点, 有效范围 " 
            << MIN_FREQ/1000000 << "-" << MAX_FREQ/1000000 << " MHz)";
    }
    
    return {status, oss.str()};
}

// RK3588 串口设备检测
SystemCheckResult check_serial_devices() {
    log_message("执行串口设备检测");
    
    vector<string> detected_ports;
    
    for (const auto& port : SERIAL_PORTS) {
        if (fs::exists(port)) {
            int fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
            if (fd != -1) {
                detected_ports.push_back(port);
                close(fd);
            }
        }
    }
    
    if (!detected_ports.empty()) {
        ostringstream oss;
        oss << "检测到串口设备: ";
        for (size_t i = 0; i < detected_ports.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << detected_ports[i];
        }
        return {"success", oss.str()};
    }
    
    return {"warning", "未检测到串口设备"};
}

// RK3588 相机设备检测
SystemCheckResult check_camera_devices() {
    log_message("执行相机设备检测");
    
    vector<string> camera_devices;
    
    // 检测CSI相机
    for (int i = 0; i < 10; ++i) {
        string device_path = "/dev/video" + to_string(i);
        if (fs::exists(device_path)) {
            int fd = open(device_path.c_str(), O_RDONLY);
            if (fd != -1) {
                v4l2_capability capability;
                if (ioctl(fd, VIDIOC_QUERYCAP, &capability) != -1) {
                    if (capability.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
                        camera_devices.push_back(device_path);
                    }
                }
                close(fd);
            }
        }
    }
    
    // 检测USB相机
    for (int i = 10; i < 20; ++i) {
        string device_path = "/dev/video" + to_string(i);
        if (fs::exists(device_path)) {
            int fd = open(device_path.c_str(), O_RDONLY);
            if (fd != -1) {
                camera_devices.push_back(device_path);
                close(fd);
            }
        }
    }
    
    if (!camera_devices.empty()) {
        ostringstream oss;
        oss << "检测到相机设备: ";
        for (size_t i = 0; i < camera_devices.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << camera_devices[i];
        }
        return {"success", oss.str()};
    }
    
    return {"warning", "未检测到相机设备"};
}


// 创建JSON响应
string create_json_response(const SystemCheckResult& result) {
    ostringstream oss;
    oss << "{\"status\":\"" << result.status << "\", \"message\":\"" << result.message << "\"}";
    return oss.str();
}

// 获取所有可用图片
vector<string> get_available_images() {
    vector<string> images;
    DIR *dir;
    struct dirent *ent;
    
    log_message("扫描图片目录: " + IMAGE_DIRECTORY);
    
    if ((dir = opendir(IMAGE_DIRECTORY.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            string filename = ent->d_name;
            // 只添加图片文件
            if (filename.find(".jpg") != string::npos || 
                filename.find(".jpeg") != string::npos ||
                filename.find(".png") != string::npos) {
                images.push_back(filename);
                log_message("发现图片文件: " + filename);
            }
        }
        closedir(dir);
    } else {
        log_message("无法打开图片目录: " + IMAGE_DIRECTORY);
    }
    
    log_message("找到 " + to_string(images.size()) + " 张图片");
    return images;
}

// 处理图片请求
void handle_image_request(int client_socket, const string& path) {
    const string api_path = "/api/v1/image/";
    const size_t api_path_len = api_path.length();
    
    log_message("处理图片请求: " + path);
    
    // 检测是否是列表请求或无效路径
    size_t pos = path.find(api_path);
    if (pos == string::npos) {
        // 请求图片列表的特殊端点
        if (path == "/api/v1/image" || path == "/api/v1/image/") {
            log_message("请求图片列表");
            vector<string> images = get_available_images();
            if (images.empty()) {
                log_message("没有可用图片");
                string response = HTTP_NOT_FOUND + "{\"error\":\"No images available\"}";
                send(client_socket, response.c_str(), response.size(), 0);
                return;
            }
            
            ostringstream oss;
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
            
            string response = oss.str();
            log_message("返回图片列表: " + to_string(images.size()) + " 个文件");
            send(client_socket, response.c_str(), response.size(), 0);
            return;
        }
        
        // 无效的请求路径
        log_message("无效的图片请求路径: " + path);
        string response = HTTP_ERROR_HEADER + "{\"error\":\"Invalid image request path\"}";
        send(client_socket, response.c_str(), response.size(), 0);
        return;
    }
    
    // 提取文件名（跳过 API 路径部分）
    string filename = path.substr(pos + api_path_len);
    log_message("请求图片文件: " + filename);
    
    // 安全检测：只防止路径遍历攻击（../）不禁止点号
    if (filename.find("..") != string::npos) {
        log_message("检测到非法路径字符: " + filename);
        string response = HTTP_ERROR_HEADER + "{\"error\":\"Invalid filename\"}";
        send(client_socket, response.c_str(), response.size(), 0);
        return;
    }
    
    string image_path = IMAGE_DIRECTORY + filename;
    log_message("查找图片: " + image_path);
    
    // 检查文件是否存在
    if (!fs::exists(image_path)) {
        log_message("图片文件不存在: " + image_path);
        string response = HTTP_NOT_FOUND + "{\"error\":\"Image not found: " + filename + "\"}";
        send(client_socket, response.c_str(), response.size(), 0);
        return;
    }
    
    // 检查是否是目录
    if (fs::is_directory(image_path)) {
        log_message("路径是目录: " + image_path);
        string response = HTTP_ERROR_HEADER + "{\"error\":\"Requested path is a directory\"}";
        send(client_socket, response.c_str(), response.size(), 0);
        return;
    }
    
    log_message("图片存在: " + image_path + " 大小: " + to_string(fs::file_size(image_path)) + " bytes");
    
    // 打开图片文件
    ifstream file(image_path, ios::binary);
    if (!file) {
        log_message("无法打开图片文件: " + image_path + ", 错误: " + strerror(errno));
        string response = HTTP_ERROR_HEADER + "{\"error\":\"Failed to open image\"}";
        send(client_socket, response.c_str(), response.size(), 0);
        return;
    }
    
    // 获取文件大小
    file.seekg(0, ios::end);
    streamsize size = file.tellg();
    file.seekg(0, ios::beg);
    
    // 确定内容类型（根据文件扩展名）
    string content_type = "image/jpeg";
    string ext = fs::path(filename).extension().string();
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".png") {
        content_type = "image/png";
    } else if (ext == ".gif") {
        content_type = "image/gif";
    } else if (ext == ".bmp") {
        content_type = "image/bmp";
    } else if (ext == ".webp") {
        content_type = "image/webp";
    }
    
    log_message("内容类型: " + content_type + ", 文件大小: " + to_string(size) + " bytes");
    
    // 构建响应头
    ostringstream header_stream;
    header_stream << "HTTP/1.1 200 OK\r\n"
                  << "Content-Type: " << content_type << "\r\n"
                  << "Content-Length: " << size << "\r\n"
                  << "Access-Control-Allow-Origin: *\r\n"
                  // 添加这行，强制浏览器下载文件
                  << "Content-Disposition: attachment; filename=\"" << filename << "\"\r\n"
                  << "Connection: close\r\n\r\n";
    
    string header = header_stream.str();
    
    log_message("发送响应头: " + string(header).substr(0, 100) + "...");
    
    // 发送响应头
    ssize_t sent = send(client_socket, header.c_str(), header.size(), 0);
    if (sent <= 0) {
        log_message("发送响应头失败: " + string(strerror(errno)));
        file.close();
        return;
    }
    
    // 发送图片内容（使用更大缓冲区提高效率）
    const int BUFFER_SIZE = 65536;  // 64KB 缓冲区
    vector<char> buffer(BUFFER_SIZE);
    streamsize total_sent = 0;
    streamsize bytes_read = 0;
    
    while (file) {
        file.read(buffer.data(), BUFFER_SIZE);
        bytes_read = file.gcount();
        
        if (bytes_read > 0) {
            ssize_t bytes_sent = send(client_socket, buffer.data(), bytes_read, 0);
            if (bytes_sent <= 0) {
                log_message("发送图片数据中断: " + string(strerror(errno)));
                break;
            }
            total_sent += bytes_sent;
        }
    }
    
    file.close();
    log_message("发送图片完成: " + filename + "， 发送字节: " + to_string(total_sent) + "/" + to_string(size));
}


// 处理GET请求
string handle_get_request(const string& path) {
    log_message("处理GET请求: " + path);
    
    if (path == "/api/v1/check/ssh") {
        return HTTP_OK_HEADER + create_json_response(check_ssh_connection());
    } else if (path == "/api/v1/check/memory") {
        return HTTP_OK_HEADER + create_json_response(check_memory_usage());
    } else if (path == "/api/v1/check/gpu") {
        return HTTP_OK_HEADER + create_json_response(check_gpu_frequency());
    } else if (path == "/api/v1/check/serial") {
        return HTTP_OK_HEADER + create_json_response(check_serial_devices());
    } else if (path == "/api/v1/check/camera") {
        return HTTP_OK_HEADER + create_json_response(check_camera_devices());
    } else if (path == "/") {
        return HTTP_OK_HEADER + "{\"message\":\"飞行系统状态检测API已运行\"}";
    } else {
        return HTTP_NOT_FOUND + "{\"error\":\"Endpoint not found\"}";
    }
}

// 连接处理函数
void handle_connection(int client_socket) {
    char buffer[4096] = {0};
    ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer));
    
    if (bytes_read < 0) {
        perror("read");
        close(client_socket);
        return;
    }
    
    string request(buffer, bytes_read);
    log_message("收到请求: " + request.substr(0, 200) + "...");
    
    // 解析请求方法和路径
    string method, path, http_version;
    istringstream request_stream(request);
    request_stream >> method >> path >> http_version;
    
    log_message("解析请求: " + method + " " + path + " " + http_version);
    
    string response;
    
    // 处理OPTIONS请求 
    if (method == "OPTIONS") {
        log_message("处理OPTIONS请求");
        response = HTTP_NO_CONTENT;
        send(client_socket, response.c_str(), response.size(), 0);
        close(client_socket);
        return;
    } 

    // 处理图片请求
    if (method == "GET" && path.find("/api/v1/image") == 0) {
        log_message("转发到图片处理");
        handle_image_request(client_socket, path);
        close(client_socket);
        return;
    }

    // 处理其他请求
    try {
        if (method == "GET") {
            log_message("转发到GET处理");
            response = handle_get_request(path);
        } else {
            log_message("不支持的HTTP方法: " + method);
            response = HTTP_ERROR_HEADER + "{\"error\":\"Unsupported HTTP method\"}";
        }
    } catch (const exception& e) {
        log_message("处理请求时出错: " + string(e.what()));
        response = HTTP_ERROR_HEADER + "{\"error\":\"Internal server error\"}";
    }

    log_message("发送响应: " + response.substr(0, 100) + "...");
    send(client_socket, response.c_str(), response.size(), 0);
    close(client_socket);
    log_message("连接关闭");
}

int main(int argc, char* argv[]) {
    int port = 8080;
    if (argc > 1) {
        port = stoi(argv[1]);
    }
    
    // 打印当前工作目录
    string current_path = fs::current_path().string();
    log_message("当前工作目录: " + current_path);
    
    // 创建图片目录（如果不存在） - 使用递归创建确保目录存在
    if (!fs::exists(IMAGE_DIRECTORY)) {
        try {
            if (fs::create_directories(IMAGE_DIRECTORY)) {
                auto abs_path = fs::absolute(IMAGE_DIRECTORY);
                log_message("已创建图片目录: " + abs_path.string());
                cout << "已创建图片目录: " << abs_path << endl;
            } else {
                log_message("警告: 无法创建图片目录 " + IMAGE_DIRECTORY);
                cerr << "警告: 无法创建图片目录 " << IMAGE_DIRECTORY << endl;
            }
        } catch (const exception& e) {
            log_message("创建图片目录失败: " + string(e.what()));
            cerr << "错误: " << e.what() << endl;
        }
    }
    
    // 打印图片目录绝对路径
    auto img_abs_path = fs::absolute(IMAGE_DIRECTORY);
    log_message("图片存储路径: " + img_abs_path.string());
    
    // 列出图片目录内容（调试用）
    log_message("图片目录内容:");
    system(("ls -l " + IMAGE_DIRECTORY + " 2>/dev/null").c_str());
    
    log_message("启动飞行系统状态检测服务器，端口: " + to_string(port));
    
    int server_socket;
    struct sockaddr_in server_addr;
    
    // 创建socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // 设置套接字选项允许地址重用
    int reuse = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    // 配置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    // 绑定socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    // 开始监听
    if (listen(server_socket, 10) < 0) {
        perror("listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    log_message("服务器已启动，等待连接...");
    cout << "API列表：" << endl;
    cout << "  SSH状态:       http://localhost:" << port << "/api/v1/check/ssh" << endl;
    cout << "  内存状态:      http://localhost:" << port << "/api/v1/check/memory" << endl;
    cout << "  GPU频率:       http://localhost:" << port << "/api/v1/check/gpu" << endl;
    cout << "  串口设备状态:  http://localhost:" << port << "/api/v1/check/serial" << endl;
    cout << "  相机设备状态:  http://localhost:" << port << "/api/v1/check/camera" << endl;
    cout << "  获取图片列表:  http://localhost:" << port << "/api/v1/image" << endl;
    cout << "  图片目录位置:  " << img_abs_path << endl;
    cout << "  当前工作目录:  " << current_path << endl;
    
    
    // 服务器主循环
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            perror("accept");
            continue;
        }
        
        // 使用手动解析IP函数
        string client_ip = ip_to_string(client_addr.sin_addr);
        log_message("接受来自 " + client_ip + " 的连接");
        
        // 创建线程处理连接
        thread(handle_connection, client_socket).detach();
    }
    
    close(server_socket);
    return 0;
}