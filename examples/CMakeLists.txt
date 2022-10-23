cmake_minimum_required(VERSION 2.8)

# Define project name
project(examples)

# Include
SET(INCLUDE_LIST ${DLAS_INCLUDE_DIR})
message(STATUS "include: ${INCLUDE_LIST}")
include_directories(${INCLUDE_LIST})

# # Source
# file(GLOB_RECURSE EXAMPLE_SRC_LIST 
#     "${PROJECT_SOURCE_DIR}/*.cpp"
# )
# message(STATUS "test src: ${EXAMPLE_SRC_LIST}")

# Build
add_executable(demo.out ${CMAKE_CURRENT_LIST_DIR}/demo.cpp)
target_link_libraries(demo.out dlas)

# Set output path
set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR})