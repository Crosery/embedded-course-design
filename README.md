# 嵌入式课程设计

[![Platform: ARM](https://img.shields.io/badge/Platform-ARM-blue.svg)](https://www.arm.com/)[![Build: CMake](https://img.shields.io/badge/Build-CMake-green.svg)](https://cmake.org/)[![Compiler: GCC 4.5.1](https://img.shields.io/badge/Compiler-GCC%204.5.1-orange.svg)](https://gcc.gnu.org/)[![Framework: Qt 4.7.0](https://img.shields.io/badge/Framework-Qt%204.7.0-41CD52.svg)](https://www.qt.io/)

### 项目概述

本项目是长江大学的嵌入式系统课程设计，包含了三个课设需要完成的人物内容，专为对应实验箱中的ARM Cortex-A8开发板设计：

1. **逆序流水灯控制** - GPIO硬件控制演示
2. **SQLite数据库操作** - 嵌入式数据库应用
3. **Qt图形界面LED控制** - 跨平台GUI应用

### 功能特性

#### 逆序流水灯

- 8个LED灯的逆序流水效果
- 直接GPIO寄存器操作
- 内存映射硬件控制
- 实时控制台输出

#### SQLite数据库

- 完整的CRUD操作演示
- 交互式命令行界面
- 批量数据操作
- 错误处理和恢复

#### Qt GUI控制面板

- 直观的LED控制界面
- 实时硬件状态反馈
- 中文界面支持
- 一键全部控制功能

### 系统架构

```
嵌入式课设/
├── 逆序流水灯/          # LED流水灯控制模块
│   ├── src/
│   │   └── led8.c       # 主程序和GPIO控制
│   ├── include/
│   │   └── led8.h       # 硬件定义和函数声明
│   ├── build_arm.sh     # ARM交叉编译脚本
│   └── CMakeLists.txt   # CMake构建配置
├── 数据库/              # SQLite数据库模块
│   ├── src/
│   │   ├── main.c       # 主程序和菜单系统
│   │   ├── sqlite_demo.c # 数据库操作实现
│   │   └── sqlite3.c    # SQLite源码
│   ├── include/
│   │   ├── sqlite_demo.h
│   │   └── sqlite3.h
│   └── build_arm.sh
├── QT/                  # Qt图形界面模块
│   ├── src/
│   │   └── main.cpp     # Qt应用程序
│   ├── include/
│   │   └── mainwindow.h
│   ├── qt_libs/         # Qt运行时库
│   ├── run.sh           # 一体化部署脚本
│   └── CMakeLists.txt
└── env/                 # 开发环境
    ├── toolchain-arm.cmake # ARM工具链配置
    ├── FriendlyARM/     # ARM交叉编译工具
    └── QT4.7.0/         # Qt开发环境
```

### 硬件要求

- **开发板**: ARM Cortex-A8 (如FriendlyARM Tiny210)
- **GPIO**: GPE0和GPE1端口
- **LED**: 8个LED灯连接到相应GPIO引脚
- **存储**: 最少64MB RAM，8MB Flash
- **OS**: Linux内核支持

### 开发环境

#### 编译工具链

- **交叉编译器**: ARM-Linux GCC/G++ 4.5.1
  - 用于在x86主机上编译ARM目标平台代码
  - 支持C和C++语言开发
  - 编译后的程序可在FriendlyARM开发板上运行

#### 开发框架

- **Qt 4.7.0**
  - 跨平台的C++图形用户界面应用程序开发框架
  - 已针对ARM平台交叉编译
  - 提供强大的GUI组件库和事件处理机制
  - 本项目使用了QtCore和QtGui核心模块

#### 构建系统

- **CMake**
  - 通过toolchain-arm.cmake配置实现跨平台交叉编译
  - 自动化构建流程，简化依赖管理
  - 支持构建脚本自定义(build_arm.sh)

### 安装指南

#### 1. 环境准备

**Ubuntu/Debian系统:**

```bash
# 更新包管理器
sudo apt update

# 安装CMake
sudo apt install cmake

# 安装32位库支持
sudo apt-get install libc6:i386 libstdc++6:i386 zlib1g:i386

# (可选) 使用国内镜像源加速
bash <(curl -sSL https://linuxmirrors.cn/main.sh)
```

**Arch Linux系统:**

```bash
# 更新系统
sudo pacman -Syu

# 安装CMake
sudo pacman -S cmake

# 安装32位库支持
sudo pacman -S --needed lib32-glibc lib32-zlib lib32-gcc-libs
```

#### 2. 下载环境包

env.zip的环境包下载地址 [长江大学课设环境_飞桨AI Studio星河社区](https://aistudio.baidu.com/datasetdetail/342360)

下载完解压放进根目录

```bash
# 解压环境包
unzip env.zip

# 赋予env文件夹里面的所有可执行程序权限
sudo chmod -R +x env
```

#### 3. 项目构建

**逆序流水灯项目:**

```bash
cd 逆序流水灯

./build_arm.sh
```

**数据库项目:**

```bash
cd 数据库

./build_arm.sh
```

**Qt GUI项目:**

```bash
cd QT

./run.sh
```

### 使用方法

#### 逆序流水灯

```bash
# 给予权限
chmod +x led

# 在开发板上直接运行
./led
```

程序将显示LED1-8的逆序流水效果，每个LED亮1秒。

#### 数据库演示

```bash
# 给予权限
chmod +x led

# 在开发板上直接运行
./sql_demo
```

#### Qt GUI控制

```bash
# 解压部署包
tar -xzf led_package.tar.gz

# 进入打包的目录
cd led_package

# 给予权限
chmod +x start.sh

# 直接运行
sh start.sh
```
