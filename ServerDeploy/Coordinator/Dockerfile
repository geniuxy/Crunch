FROM python:3.9-slim

# 教程内原版指令 begin
#RUN apt-get update
#
#RUN apt-get install -y curl
#
#RUN curl -fsSL https://get.docker.com | sh
# 教程内原版指令 end

# 1. 换国内 apt 源（加速系统包下载，可选）
RUN sed -i 's/deb.debian.org/mirrors.aliyun.com/g' /etc/apt/sources.list.d/debian.sources 2>/dev/null || \
    sed -i 's/deb.debian.org/mirrors.aliyun.com/g' /etc/apt/sources.list 2>/dev/null || true

# 2. 安装依赖
RUN apt-get update && apt-get install -y \
    ca-certificates curl gnupg lsb-release \
    && rm -rf /var/lib/apt/lists/*

# 3. 添加阿里云 Docker CE 源
RUN install -m 0755 -d /etc/apt/keyrings \
    && curl -fsSL https://mirrors.aliyun.com/docker-ce/linux/debian/gpg \
       -o /etc/apt/keyrings/docker.asc \
    && chmod a+r /etc/apt/keyrings/docker.asc \
    && echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.asc] \
       https://mirrors.aliyun.com/docker-ce/linux/debian \
       $(. /etc/os-release && echo "$VERSION_CODENAME") stable" \
       > /etc/apt/sources.list.d/docker.list

# 4. 安装 Docker CLI（连接宿主机用）或完整引擎
RUN apt-get update && apt-get install -y \
    docker-ce-cli \
    docker-compose-plugin \
    && rm -rf /var/lib/apt/lists/*

# docker-ce-cli：只有 docker 命令，能发指令、
# 容器内的 docker 命令并没有自己的 Docker 引擎，它借用的是宿主机的引擎

COPY . /

WORKDIR /coordinator

RUN pip3 install --no-cache-dir flask

ENTRYPOINT ["python", "coordinator.py"]