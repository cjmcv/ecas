cmake_minimum_required(VERSION 3.2)

# Define project name
project(examples)

# Include
SET(INCLUDE_LIST ${ECAS_INCLUDE_DIR})
message(STATUS "include: ${INCLUDE_LIST}")
include_directories(${INCLUDE_LIST})

# Source
file(GLOB_RECURSE EXAMPLE_SRC_LIST 
    "${PROJECT_SOURCE_DIR}/*.cpp"
)
message(STATUS "test src: ${EXAMPLE_SRC_LIST}")

# Build
# add_executable(demo ${CMAKE_CURRENT_LIST_DIR}/main.cpp)
add_executable(demo ${EXAMPLE_SRC_LIST})

target_link_libraries(demo ecas)

# Set output path
set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR})