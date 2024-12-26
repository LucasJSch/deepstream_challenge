# Start from the NVIDIA DeepStream base image
FROM nvcr.io/nvidia/deepstream:7.1-triton-multiarch

# Set the working directory
WORKDIR /root

# Install tmux and build-essential (for cmake and make) and Python pip
RUN apt-get update && \
    apt-get install -y \
    tmux \
    build-essential \
    git \
    python3-pip \
    cmake \
    libssl-dev \
    libsasl2-dev \
    python3-dev \
    openjdk-11-jdk


# Install confluent-kafka python package
RUN pip3 install confluent-kafka

# Create /opt/kafka directory
RUN mkdir -p /opt/kafka

# Download Kafka and extract it directly into /opt/kafka
RUN wget https://archive.apache.org/dist/kafka/3.4.0/kafka_2.12-3.4.0.tgz -P /opt/kafka && \
    cd /opt/kafka && \
    tar -xvzf kafka_2.12-3.4.0.tgz --strip-components=1 # Extract files directly into /opt/kafka


# Clone the librdkafka repository into /opt/kafka
RUN git clone https://github.com/confluentinc/librdkafka.git /opt/kafka-cpp

# Build librdkafka
WORKDIR /opt/kafka-cpp
RUN mkdir build && \
    cd build && \
    cmake .. && \
    make

WORKDIR /opt/nvidia/deepstream/deepstream-7.1/optriment_pipeline
# Set the default command (to keep the container running, you can adjust it as necessary)
CMD ["bash"]