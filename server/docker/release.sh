#!/bin/bash

# 指定仓库
REGISTRY="liuhuapiaoyuan/esp-ai"

# 获取当前日期和时间作为标签
DATE_TAG=$(date +%Y%m%d%H%M%S)

# 构建 Docker 镜像
docker build -t $REGISTRY:$DATE_TAG -t $REGISTRY:latest -f ./docker/Dockerfile ./

# 推送镜像到仓库
docker push $REGISTRY:$DATE_TAG
docker push $REGISTRY:latest

echo "Image built and pushed successfully with tags: $DATE_TAG and latest"
