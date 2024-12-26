## Important

The code was only tested in the Dockerized environment in WSL2 using Ubuntu 22.04.

These instructions follow the steps to use the code using a dockerized environment.

If you use a different environment, take into account that you may have to fix some references in the CMakeLists.txt file or the config files.

## Instructions

### Build & run the docker image

```
cd ./docker
./build.sh
./run.sh
```

### Run the out-of-the-box script

Once you're inside the docker container, run the following script to initialize Kafka server and listener:

```
./run.sh
```

### Build the pipeline

After running the previous step, you should be in the path `/opt/nvidia/deepstream/deepstream-7.1/optriment_pipeline`.

```
cd optriment_pipeline
mkdir build
cd build
cmake ..
make
```

## Execute the pipeline

Important: Wait for the Kafka server and listener to be up and running before running the `optriment_pipeline` executable.

Inside the build folder:

```
./optriment_pipeline
```

Or with the explicit config files:
```
./optriment_pipeline --config ../config/pipeline_config.yml --roi1 ../config/config_roi1.txt --roi2 ../config/config_roi2.txt
```




