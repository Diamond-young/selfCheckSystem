cmake_minimum_required(VERSION 3.10)
project(RK3588_System_Check)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 yaml-cpp 库
find_package(yaml-cpp REQUIRED)

# 设置编译器选项
add_compile_options(-Wall -Wextra -Wpedantic -O2)

# 添加所有头文件路径
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/config
    ${CMAKE_SOURCE_DIR}/src/network
    ${CMAKE_SOURCE_DIR}/src/system
    ${CMAKE_SOURCE_DIR}/src/types
    ${CMAKE_SOURCE_DIR}/src/utils
)


# 添加所有源文件（按照项目结构图）
file(GLOB_RECURSE SOURCE_FILES
    "src/*.cpp"
    "src/config/*.cpp"
    "src/network/*.cpp"
    "src/system/*.cpp"
    "src/utils/*.cpp"
)


# 添加目标可执行文件
add_executable(system_check_server ${SOURCE_FILES})

# 链接系统库
target_link_libraries(system_check_server PRIVATE 
    yaml-cpp
    pthread
    ${CMAKE_DL_LIBS}
)

# 为 Linux 平台添加系统特定的链接选项
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(system_check_server PRIVATE dl)
endif()

# 安装规则
install(TARGETS system_check_server DESTINATION bin)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/configs/ DESTINATION configs)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/images/ DESTINATION images)