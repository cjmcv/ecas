cmake_minimum_required(VERSION 2.8)

# # Include
# SET(INCLUDE_DIR
#     ${CMAKE_CURRENT_SOURCE_DIR}
# )
# message(STATUS "include: ${INCLUDE_DIR}")
# include_directories(${INCLUDE_DIR})
	
# Source
file(GLOB_RECURSE SRC_LIST 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" # PROJECT_SOURCE_DIR
    # Include
    # "${INCLUDE_DIR}/*.h"
)
message(STATUS "src: ${SRC_LIST}")

add_library(DLAS_VULKAN OBJECT ${SRC_LIST})