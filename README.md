## Important

The code was only tested in the Dockerized environment in WSL2 using Ubuntu 22.04. Therefore these instructions follow the steps to use the code using that environment.

If you use a different environment, take into account that you may have to fix some references in the CMakeLists.txt file or the config files.

## Instructions

We'll follow the Docker image 

### Download Docker image

Download the Docker image following [the official Deepstream documentation](https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_docker_containers.html#a-docker-container-for-dgpu).

E.g. `sudo docker pull nvcr.io/nvidia/deepstream:7.1-triton-multiarch`

### Run the Docker container

```
sudo docker run -it -v ./optriment_pipeline:/opt/nvidia/deepstream/deepstream-7.1/optriment_pipeline --privileged --rm --name=docker --net=host --gpus all -e DISPLAY=$DISPLAY -e CUDA_CACHE_DISABLE=0 --device /dev/snd -v /tmp/.X11-unix/:/tmp/.X11-unix  nvcr.io/nvidia/deepstream:7.1-triton-multiarch 
```

### Optional: Install tmux

We'll be using several terminals at once.
For ease of use, we recommend installing and runing `tmux`, using:

```
apt install tmux
tmux
```

Alternatively, you can run four terminals connected to the docker instance.

#### How to open new tabs in tmux

Press `Ctrl + B + %` to open new tabs.

#### How to switch between tabs

Press `Ctrl + B + <arrow key>`


[Check tmux's quick guide](https://hamvocke.com/blog/a-quick-and-easy-guide-to-tmux/).

## Install & run Kafka

TODO: Make Docker image with Kafka already installed.

Inside the docker container:
1. Install Python's Kafka API: ```pip install confluent-kafka```
2. Follow [these instructions](https://hostman.com/tutorials/install-apache-kafka-on-ubuntu-22-04/) to install and run Kafka in your docker instance.


## Run the Kafka listener with Python

Inside the docker container:
```
cd optriment_pipeline
python3 kafka_container.py
```

## Build the pipeline

Inside the docker container:
```
cd optriment_pipeline
mkdir build
cd build
cmake ..
make
```

## Execute the pipeline

Inside the docker container:
```
./optriment_pipeline --config ../config/pipeline_config.yml --roi1 ../config/config_roi1.txt --roi2 ../config/config_roi2.txt
```




