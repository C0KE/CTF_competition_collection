# 使用一个基础的 Python 镜像作为基础
FROM python:3.9-slim

# 设置工作目录
WORKDIR /app

# 复制当前目录下的所有文件到容器中的 /app 目录

# 安装所需的依赖包
RUN pip install --no-cache-dir flask && \
	mkdir -p /app/templates && \
	pip install simple-websocket



# 暴露容器的端口
EXPOSE 5000

# 运行 Flask 应用




# 拷贝你的PHP代码到Apache的Web服务器目录
COPY ./kernel /app/templates
COPY ./flag /app/
COPY ./index.html /app/templates
COPY ./app.py /app

RUN chmod +x /app/templates/kernel

# 启动Apache服务器
CMD ["python", "app.py"]
