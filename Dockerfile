FROM ubuntu:24.04

# 设置非交互式环境和编码
ENV DEBIAN_FRONTEND=noninteractive
ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8
ENV LANGUAGE=C.UTF-8

# 更换镜像源
RUN echo "deb http://mirrors.aliyun.com/ubuntu/ noble main restricted universe multiverse" > /etc/apt/sources.list && \
    echo "deb http://mirrors.aliyun.com/ubuntu/ noble-security main restricted universe multiverse" >> /etc/apt/sources.list && \
    echo "deb http://mirrors.aliyun.com/ubuntu/ noble-updates main restricted universe multiverse" >> /etc/apt/sources.list && \
    echo "deb http://mirrors.aliyun.com/ubuntu/ noble-backports main restricted universe multiverse" >> /etc/apt/sources.list && \
    # 删除存在的独立security配置
    rm -f /etc/apt/sources.list.d/ubuntu.sources /etc/apt/sources.list.d/security.sources

# 添加工作目录
WORKDIR /app

# 将项目文件夹复制到工作目录里面
COPY ./逆序流水灯/ /app/逆序流水灯/
COPY ./QT/ /app/QT/
COPY ./数据库/ /app/数据库/
COPY ./env/ /app/env/
COPY ./start.sh /app/start.sh

RUN dpkg --add-architecture i386 && \
    apt-get update && \
    apt-get install -y \
    cmake \
    build-essential \
    make \
    wget \
    curl \
    unzip \
    git \
    sudo \
    libncurses5-dev \
    libc6:i386 \
    libstdc++6:i386 \
    zlib1g:i386 \
    dos2unix \
    && rm -rf /var/lib/apt/lists/*

# 修复脚本文件的换行符问题
RUN find /app -type f -name "*.sh" -exec dos2unix {} \;

# 给ENV环境所有可执行权限
RUN chmod -R +x /app/env

# 赋予执行权限
RUN chmod +x /app/start.sh
RUN chmod +x /app/QT/*.sh
RUN chmod +x /app/逆序流水灯/*.sh
RUN chmod +x /app/数据库/*.sh


# 默认命令
CMD ["/bin/bash"]
