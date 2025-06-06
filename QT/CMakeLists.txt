# CMake最小版本要求
cmake_minimum_required(VERSION 3.20.0)

# 项目名称
project(embedded_lab)

# 设置C++标准
set(CMAKE_CXX_STANDARD 98) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt4组件
# (所有路径查找的工作已经完全交给 toolchain-arm.cmake)
find_package(Qt4 REQUIRED QtCore QtGui)

# 包含Qt4的CMake模块
include(${QT_USE_FILE})

# 添加Qt4定义
add_definitions(${QT_DEFINITIONS})

# 添加头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${QT_INCLUDES})

# 设置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 指定源文件和头文件
set(SRCS
    src/main.cpp
)

set(HEADERS
    include/mainwindow.h
)

# 使用Qt4的MOC处理
set(MOC_FILES "")
foreach(HEADER ${HEADERS})
    get_filename_component(HEADER_NAME ${HEADER} NAME_WE)
    set(MOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/${HEADER_NAME}_moc.cpp)
    add_custom_command(
        OUTPUT ${MOC_FILE}
        COMMAND ${QT_MOC_EXECUTABLE} 
            ${CMAKE_CURRENT_SOURCE_DIR}/${HEADER} 
            -o ${MOC_FILE}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${HEADER}
    )
    list(APPEND MOC_FILES ${MOC_FILE})
endforeach()

# 添加可执行文件
add_executable(
    embedded_lab
    ${SRCS}
    ${MOC_FILES}
)

# 链接Qt库
target_link_libraries(embedded_lab ${QT_LIBRARIES})

# 生成compile_commands.json帮助VSCode识别头文件
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
