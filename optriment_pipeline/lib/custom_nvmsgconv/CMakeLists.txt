# Set the project name and version
cmake_minimum_required(VERSION 3.10)
project(nvds_msgconv_custom VERSION 6.1)

# Set the package dependencies
find_package(PkgConfig REQUIRED)
#pkg_check_modules(PACKAGES glib-2.0 gobject-2.0 json-glib-1.0 uuid gio-2.0)
pkg_check_modules(PACKAGES glib-2.0 gobject-2.0 json-glib-1.0 gio-2.0)

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
set(GST_INSTALL_DIR "/opt/nvidia/deepstream/deepstream-7.1/lib/gst-plugins/")
set(GST_INSTALL_DIR2 "/opt/nvidia/deepstream/deepstream-7.1/lib/")

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
    /opt/nvidia/deepstream/deepstream-7.1/sources/libs
    /opt/nvidia/deepstream/deepstream-7.1/sources/includes
    /usr/include/json-glib-1.0
    /usr/include/libmount
    /usr/include/blkid
    /usr/include/glib-2.0
    /usr/lib/x86_64-linux-gnu/glib-2.0/include
    #/usr/include/uuid
)

# Link directories (if required, you can add them here)
link_directories(
    ${LIB_INSTALL_DIR}
    /usr/local/cuda-${CUDA_VER}/lib64/
    ${LIB_INSTALL_DIR_2}
    /sources/libs
    #${glib_2_0_LIBRARY_DIRS}
    #${gobject_2_0_LIBRARY_DIRS}
    #${json_glib_1_0_LIBRARY_DIRS}
    #${uuid_LIBRARY_DIRS}
    #${gio-2_0_LIBRARY_DIRS}
    /usr/lib/x86_64-linux-gnu
)

# Link with GStreamer and CUDA libraries
set(LIBS
    ${GSTREAMER_LIBRARIES}
    -lcudart
    -lnvdsgst_meta
    -lnvds_meta
    -lnvds_yml_parser
    -lnvdsgst_helper
    -lnvdsgst_3d_gst
    -lnvds_3d_common
    -ldl
    -lgio-2.0
    #-luuid
    -ljson-glib-1.0
    -lglib-2.0
    -Wl,-rpath,${LIB_INSTALL_DIR}
)

# Define the target library
add_library(nvds_msgconv_custom SHARED ${SRCS})

# Link the required libraries
target_link_libraries(nvds_msgconv_custom
    yaml-cpp
    ${LIBS}
    #${gio_2_0_LIBRARIES}
    #${glib_2_0_LIBRARIES}
    #${gobject_2_0_LIBRARIES}
    #${json_glib_1_0_LIBRARIES}
    #${uuid_LIBRARIES}
    ##glib-2.0
    ##gobject-2.0
    ##gio-2.0
    ##json-glib-1.0
    ##uuid
)

# Install the plugin library (shared object .so) to the GStreamer plugin directory
install(TARGETS nvds_msgconv_custom
    LIBRARY DESTINATION ${GST_INSTALL_DIR}
)
install(TARGETS nvds_msgconv_custom
    LIBRARY DESTINATION ${GST_INSTALL_DIR2}
)
