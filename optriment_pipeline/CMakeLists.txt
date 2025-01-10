cmake_minimum_required(VERSION 3.10)
project(prototype)

set(CUDA_VER "12.6")
# Check if CUDA version is set
if(NOT DEFINED CUDA_VER)
    message(FATAL_ERROR "CUDA_VER is not set")
endif()

# Set the DeepStream version and target device
set(NVDS_VERSION "7.1")
set(TARGET_DEVICE ${CMAKE_SYSTEM_PROCESSOR})

set(CMAKE_BUILD_TYPE Debug)

# Set installation directories
set(LIB_INSTALL_DIR "/opt/nvidia/deepstream/deepstream-${NVDS_VERSION}/lib/")
set(APP_INSTALL_DIR "/opt/nvidia/deepstream/deepstream-${NVDS_VERSION}/bin/")

# Platform-specific flags for Tegra devices
if(TARGET_DEVICE STREQUAL "aarch64")
    add_definitions(-DPLATFORM_TEGRA)
endif()

# Find source files and header files
#file(GLOB SRCS "./src/*.cpp")
file(GLOB INCS "./include/*.h")


# Find GStreamer package
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)


# Set include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include 
    ../sources/includes
    /usr/local/cuda-${CUDA_VER}/include
    ${GSTREAMER_INCLUDE_DIRS}
)

# Add the CUDA libraries
link_directories(
    /usr/local/cuda-${CUDA_VER}/lib64/
    ${LIB_INSTALL_DIR_2}
    ${LIB_INSTALL_DIR}
)

# Link with GStreamer and CUDA libraries
set(LIBS
    ${GSTREAMER_LIBRARIES}
    -lcudart
    -lnvdsgst_meta
    -lnvds_meta
    -lnvds_yml_parser
    -Wl,-rpath,${LIB_INSTALL_DIR}
)

################################# BUILD MODULES #######################################

add_library(kafka_publisher STATIC src/KafkaPublisher.cpp)
target_link_libraries(kafka_publisher rdkafka++)

add_library(pipeline STATIC src/Pipeline.cpp)
target_link_libraries(pipeline ${LIBS})

add_library(region_count_message STATIC src/RegionCountMessage.cpp)
target_link_libraries(region_count_message ${LIBS})


################################# BUILD MAIN APP ######################################

# Add executable
set(APP optriment_pipeline)
add_executable(${APP} src/optriment_pipeline.cpp)

# Link the libraries to the application
target_link_libraries(${APP}
${LIBS}
kafka_publisher
pipeline
region_count_message
)

