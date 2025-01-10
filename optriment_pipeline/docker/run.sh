#!/bin/bash

SCRIPT_DIR=$(dirname "$(realpath "$0")")
PARENT_DIR=$(dirname "$SCRIPT_DIR")

IMAGE_NAME="optriment-challenge"

echo "Running Docker container: $IMAGE_NAME..."

export DISPLAY=:0
xhost +

docker run -it \
    -v $PARENT_DIR:/opt/nvidia/deepstream/deepstream-7.1/optriment_pipeline \
    --privileged \
    --rm \
    --name=docker \
    --net=host \
    --gpus all \
    -e DISPLAY=$DISPLAY \
    -e CUDA_CACHE_DISABLE=0 \
    --device /dev/snd \
    -v /tmp/.X11-unix/:/tmp/.X11-unix \
    $IMAGE_NAME
