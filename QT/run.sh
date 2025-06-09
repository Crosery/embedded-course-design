# --- helper functions for logging ---
log_info() { echo -e "\033[1;32m[信息]\033[0m $1"; }
log_error() { echo -e "\033[1;31m[错误]\033[0m $1"; }
log_step() { echo -e "\033[1;34m[步骤]\033[0m $1"; }
separator() { echo -e "\033[1;36m=================================================\033[0m"; }

# --- header ---
separator
echo -e "\033[1;36m   ARM-QT 一键编译打包脚本   \033[0m"
separator
echo

# --- 1. Clean old files ---
log_step "正在清理旧文件..."
rm -rf build led_package led_package.tar.gz
log_info "清理完成"
echo

# --- 2. Compile the program ---
log_step "正在编译具有内置Qtopia控制的程序..."
mkdir -p build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../../env/toolchain-arm.cmake ..
if [ $? -ne 0 ]; then log_error "CMake配置失败!"; exit 1; fi
make -j4
if [ $? -ne 0 ]; then log_error "编译失败!"; exit 1; fi
cd ..
log_info "编译成功"
echo

# --- 3. Prepare package with a self-contained app ---
log_step "正在准备部署包 (包含库和字体)..."
PACKAGE_DIR="led_package"
mkdir -p ${PACKAGE_DIR}/qt_libs/fonts

# Copy executable
if [ -f "bin/embedded_lab" ]; then
    cp bin/embedded_lab ${PACKAGE_DIR}/
    chmod +x ${PACKAGE_DIR}/embedded_lab
else
    log_error "未找到可执行文件 bin/embedded_lab"
    exit 1
fi

# Copy Qt libraries
log_info "正在复制Qt库..."
if [ -d "qt_libs" ]; then
    cp -r qt_libs/* ${PACKAGE_DIR}/qt_libs/
else
    log_error "未找到 'qt_libs' 目录，无法打包必要的库文件!"
    exit 1
fi

# Copy unifont for Chinese display
log_info "正在复制中文字体..."
UNIFONT_PATH="../env/qte4.7.0/qt-everywhere-opensource-src-4.7.0/lib/fonts/unifont_160_50.qpf"
if [ -f "$UNIFONT_PATH" ]; then
    cp "$UNIFONT_PATH" ${PACKAGE_DIR}/qt_libs/fonts/
fi
log_info "部署包准备完成"
echo

# --- 4. Create a minimal launcher script ---
log_step "正在创建极简启动脚本 (start.sh)..."
cat > ${PACKAGE_DIR}/start.sh << 'EOF'
#!/bin/sh

# 1. Go to the script's directory
cd $(dirname $0)

# 2. Set environment to use our local libs
export LD_LIBRARY_PATH="$(pwd)/qt_libs:/lib:/usr/lib"
export QT_QWS_FONTDIR="$(pwd)/qt_libs/fonts"

# 3. Run the application as a QWS server.
# The application will now handle suspending and resuming Qtopia by itself.
echo "--> 正在启动自包含的LED控制程序..."
./embedded_lab -qws

echo "--> 程序已退出，控制权已交还。"
EOF
chmod +x ${PACKAGE_DIR}/start.sh
log_info "启动脚本创建成功"
echo

# --- 5. Pack the files ---
log_step "正在打包文件..."
tar -czf output/led_package.tar.gz ${PACKAGE_DIR}
log_info "打包成功: output/led_package.tar.gz"
echo

# --- Final instructions ---
separator
echo -e "\033[1;36m 所有操作已完成文件已经保存到output目录 \033[0m"
separator
echo "程序现已自包含Qtopia控制逻辑。"
echo "请将 'led_package.tar.gz' 上传到开发板并解压。"s
echo "进入 'led_package' 目录，以root权限运行 './start.sh' 即可。"
echo
separator 