cmake_minimum_required(VERSION 3.2)

# Include
SET(INCLUDE_DIR
    ${VULKAN_WIN_SDK}/Include
)
message(STATUS "include: ${INCLUDE_DIR}")
include_directories(${INCLUDE_DIR})
	
# Source
file(GLOB_RECURSE SRC_LIST 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" # PROJECT_SOURCE_DIR
    # Include
    # "${INCLUDE_DIR}/*.h"
)
message(STATUS "src: ${SRC_LIST}")

add_library(ECAS_KERNEL_VULKAN OBJECT ${SRC_LIST})