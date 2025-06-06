#!/bin/bash

# ARM Qt项目构建脚本

echo "开始构建ARM程序..."

# 创建并进入构建目录
if [ -d "build" ]; then
    rm -rf build/*
else
    mkdir -p build
fi

cd build

# 使用工具链文件配置CMake
cmake .. -DCMAKE_TOOLCHAIN_FILE=../../env/toolchain-arm.cmake

# 如果配置成功，开始编译
if [ $? -eq 0 ]; then
    echo "CMake配置成功，开始编译..."
    make -j$(nproc)
    
    if [ $? -eq 0 ]; then
        echo "编译成功！"
        echo "可执行文件位于: ../bin/embedded_lab"
        echo "您可以将此文件传输到ARM开发板上运行。"
    else
        echo "编译失败！"
        exit 1
    fi
else
    echo "CMake配置失败！"
    exit 1
fi 