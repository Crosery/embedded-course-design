#!/bin/bash

echo "请选择要编译的项目:"
echo "1) QT程序"
echo "2) 数据库"
echo "3) 逆序流水灯"
echo "4) 全部编译"
echo "0) 退出"

read -p "请输入选项数字: " choice

case $choice in
    1)
        echo "开始编译QT程序..."
        cd ./QT && ./run.sh
        ;;
    2) 
        echo "开始编译数据库..."
        cd ./数据库 && ./build_arm.sh
        ;;
    3)
        echo "开始编译逆序流水灯..."
        cd ./逆序流水灯 && ./build_arm.sh
        ;;
    4)
        echo "开始全部编译..."
        cd ./QT && ./run.sh
        cd ..
        cd ./数据库 && ./build_arm.sh
        cd ..
        cd ./逆序流水灯 && ./build_arm.sh
        ;;
    0)
        echo "退出编译"
        exit 0
        ;;
    *)
        echo "无效选项!"
        exit 1
        ;;
esac