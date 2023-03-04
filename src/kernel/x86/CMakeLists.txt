cmake_minimum_required(VERSION 3.2)

# # Include
# SET(INCLUDE_DIR
#     ${CMAKE_CURRENT_SOURCE_DIR}
# )
# message(STATUS "include: ${INCLUDE_DIR}")
# include_directories(${INCLUDE_DIR})

# message(STATUS "current dir: ${CMAKE_CURRENT_SOURCE_DIR}")
# message(STATUS "project dir: ${PROJECT_SOURCE_DIR}")

# Source
file(GLOB_RECURSE SRC_LIST 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" # PROJECT_SOURCE_DIR
    # Include
    # "${INCLUDE_DIR}/*.h"
)
message(STATUS "src: ${SRC_LIST}")

add_library(ECAS_X86 OBJECT ${SRC_LIST})