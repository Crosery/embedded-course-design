# CMake最小版本要求
cmake_minimum_required(VERSION 3.20.0)

# 项目名称
project(sql_demo)

# 设置C++标准
set(CMAKE_CXX_STANDARD 98) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 设置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 指定源文件和头文件
aux_source_directory(src SRCS)
aux_source_directory(include HEADERS)

# 添加可执行文件
add_executable(
    sql_demo
    ${SRCS}
)

# 链接线程库和动态链接库
target_link_libraries(sql_demo pthread dl)
