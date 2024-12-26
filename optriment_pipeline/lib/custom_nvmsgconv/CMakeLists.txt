# Set the project name and version
cmake_minimum_required(VERSION 3.10)
project(nvds_msgconv_custom VERSION 6.1)

# Set the package dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(PACKAGES glib-2.0 gobject-2.0 json-glib-1.0 uuid)

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
file(GLOB SRCS "./src/*.cpp")

# Find GStreamer package
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include 
    /usr/local/cuda-${CUDA_VER}/include
    ${GSTREAMER_INCLUDE_DIRS}
    ./include
    ${PACKAGES_INCLUDE_DIRS}
)

# Link directories (if required, you can add them here)
link_directories(
    ${LIB_INSTALL_DIR}
    /usr/local/cuda-${CUDA_VER}/lib64/
    ${LIB_INSTALL_DIR_2}
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

# Define the target library
add_library(nvds_msgconv_custom SHARED ${SRCS})

# Link the required libraries
target_link_libraries(nvds_msgconv_custom
    yaml-cpp
    ${LIBS}
)



