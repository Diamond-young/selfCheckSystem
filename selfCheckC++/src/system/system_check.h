#ifndef SYSTEM_CHECK_H
#define SYSTEM_CHECK_H

#include "../types/common_types.h"
#include <string>
#include <vector>
#include <map>

class SystemCheck {
public:
    // SSH连接检测
    static SystemCheckResult check_ssh_connection();
    
    // 内存占用检测
    static SystemCheckResult check_memory_usage();
    
    // GPU频点设置检测
    static SystemCheckResult check_gpu_frequency();
    
    // 串口设备检测
    static SystemCheckResult check_serial_devices();
    
    // 相机设备检测
    static SystemCheckResult check_camera_devices();
    
    // 创建JSON响应
    static std::string create_json_response(const SystemCheckResult& result);

private:
    // GPU温度检测（辅助函数）
    static int get_gpu_temperature();
    
    // RK3588 GPU路径常量
    static const std::vector<std::string> GPU_TEMP_PATHS;
    static const std::vector<std::string> GPU_FREQ_PATHS;
    
    // RK3588 串口设备路径
    static const std::vector<std::string> SERIAL_PORTS;
};

#endif // SYSTEM_CHECK_H