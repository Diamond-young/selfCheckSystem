#include "system_check.h"
#include "../utils/logger.h"  // 包含Logger头文件
#include <fstream>
#include <filesystem>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sys/sysinfo.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <dirent.h>
#include <mutex>
#include <algorithm>
#include <unistd.h>  // 添加 close 函数声明
#include <cstring>    // 添加 strlen 函数声明
#include <vector>
#include <thread>     // 添加 thread 头文件

namespace fs = std::filesystem;

// RK3588 GPU温度传感器路径
const std::vector<std::string> SystemCheck::GPU_TEMP_PATHS = {
    "/sys/class/thermal/thermal_zone0/temp",
    "/sys/class/thermal/thermal_zone1/temp"
};

// RK3588 GPU频率传感器路径
const std::vector<std::string> SystemCheck::GPU_FREQ_PATHS = {
    "/sys/devices/platform/fb000000.gpu/devfreq/fb000000.gpu/cur_freq",
    "/sys/class/devfreq/fb000000.gpu/cur_freq",
    "/sys/class/misc/mali0/device/devfreq/cur_freq"
};

// RK3588 串口设备路径
const std::vector<std::string> SystemCheck::SERIAL_PORTS = {
    "/dev/ttyS0", "/dev/ttyS1", "/dev/ttyS2", 
    "/dev/ttyS3", "/dev/ttyS4", "/dev/ttyS5",
    "/dev/ttyS6", "/dev/ttyS7", "/dev/ttyS8"
};

int SystemCheck::get_gpu_temperature() {
    for (const auto& temp_path : GPU_TEMP_PATHS) {
        try {
            if (fs::exists(temp_path)) {
                std::ifstream temp_file(temp_path);
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

SystemCheckResult SystemCheck::check_gpu_frequency() {
    Logger::info("执行 RK3588 GPU频率检测");
    
    long current_freq = -1;
    std::string freq_path_used = "";

    // 按优先级尝试所有可能的路径
    for (const auto& path : GPU_FREQ_PATHS) {
        try {
            if (fs::exists(path)) {
                std::ifstream freq_file(path);
                if (freq_file.is_open()) {
                    std::string freq_str;
                    std::getline(freq_file, freq_str);
                    freq_file.close();
                    
                    // 确保读取的是有效数字
                    if (!freq_str.empty() && std::all_of(freq_str.begin(), freq_str.end(), ::isdigit)) {
                        current_freq = std::stol(freq_str);
                        freq_path_used = path;
                        break;
                    }
                }
            }
        } catch (const std::exception& e) {
            Logger::error(std::string("尝试访问 ") + path + " 时出错: " + e.what());
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
    
    std::string status = "success";
    std::string temp_info = "";
    
    if (gpu_temp > 0) {
        char buffer[50];
        snprintf(buffer, sizeof(buffer), ", 温度: %d°C", gpu_temp);
        temp_info = buffer;
        
        if (gpu_temp > 85) {
            status = "warning";
        }
    }
    
    char buffer[200];
    snprintf(buffer, sizeof(buffer), "当前GPU频率: %.2f MHz (来源: %s)%s", 
             current_freq / 1000000.0, freq_path_used.c_str(), temp_info.c_str());
    
    // 检查频率是否在合理范围
    if (current_freq < MIN_FREQ || current_freq > MAX_FREQ) {
        status = "warning";
        snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), 
                " (异常频点, 有效范围 %.0f-%.0f MHz)", 
                MIN_FREQ/1000000.0, MAX_FREQ/1000000.0);
    }
    
    return {status, buffer};
}

SystemCheckResult SystemCheck::check_serial_devices() {
    Logger::info("执行串口设备检测");
    
    std::vector<std::string> detected_ports;
    
    for (const auto& port : SERIAL_PORTS) {
        if (fs::exists(port)) {
            int fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
            if (fd != -1) {
                detected_ports.push_back(port);
                ::close(fd);  // 使用全局作用域调用
            }
        }
    }
    
    if (!detected_ports.empty()) {
        std::ostringstream oss;
        oss << "检测到串口设备: ";
        for (size_t i = 0; i < detected_ports.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << detected_ports[i];
        }
        return {"success", oss.str()};
    }
    
    return {"warning", "未检测到串口设备"};
}

SystemCheckResult SystemCheck::check_camera_devices() {
    Logger::info("执行相机设备检测");
    
    std::vector<std::string> camera_devices;
    
    // 检测CSI相机
    for (int i = 0; i < 10; ++i) {
        std::string device_path = "/dev/video" + std::to_string(i);
        if (fs::exists(device_path)) {
            int fd = open(device_path.c_str(), O_RDONLY);
            if (fd != -1) {
                struct v4l2_capability capability;
                if (ioctl(fd, VIDIOC_QUERYCAP, &capability) != -1) {
                    if (capability.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
                        camera_devices.push_back(device_path);
                    }
                }
                ::close(fd);  // 使用全局作用域调用
            }
        }
    }
    
    // 检测USB相机
    for (int i = 10; i < 20; ++i) {
        std::string device_path = "/dev/video" + std::to_string(i);
        if (fs::exists(device_path)) {
            int fd = open(device_path.c_str(), O_RDONLY);
            if (fd != -1) {
                camera_devices.push_back(device_path);
                ::close(fd);  // 使用全局作用域调用
            }
        }
    }
    
    if (!camera_devices.empty()) {
        std::ostringstream oss;
        oss << "检测到相机设备: ";
        for (size_t i = 0; i < camera_devices.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << camera_devices[i];
        }
        return {"success", oss.str()};
    }
    
    return {"warning", "未检测到相机设备"};
}

SystemCheckResult SystemCheck::check_memory_usage() {
    Logger::info("执行内存占用检测");
    
    std::ifstream meminfo_file("/proc/meminfo");
    if (!meminfo_file.is_open()) {
        return {"error", "无法打开/proc/meminfo"};
    }
    
    long total_ram = 0, free_ram = 0, buffers = 0, cached = 0;
    std::string line;
    
    while (std::getline(meminfo_file, line)) {
        if (line.find("MemTotal") != std::string::npos) {
            sscanf(line.c_str(), "MemTotal: %ld kB", &total_ram);
        } else if (line.find("MemFree") != std::string::npos) {
            sscanf(line.c_str(), "MemFree: %ld kB", &free_ram);
        } else if (line.find("Buffers") != std::string::npos) {
            sscanf(line.c_str(), "Buffers: %ld kB", &buffers);
        } else if (line.find("Cached") != std::string::npos) {
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
    
    std::string status = "success";
    if (usage_percent > 85) {
        status = "error";
    } else if (usage_percent > 70) {
        status = "warning";
    }
    
    std::ostringstream oss;
    oss << "总内存: " << total_ram/1024 << "MB, 已用: " << used_ram/1024 
        << "MB, 可用: " << available_ram/1024 << "MB, 使用率: " << usage_percent << "%";
    
    return {status, oss.str()};
}

SystemCheckResult SystemCheck::check_ssh_connection() {
    Logger::info("执行 SSH 连接检测");
    
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

std::string SystemCheck::create_json_response(const SystemCheckResult& result) {
    char buffer[512];
    snprintf(buffer, sizeof(buffer), R"({"status":"%s","message":"%s"})", 
             result.status.c_str(), result.message.c_str());
    return buffer;
}