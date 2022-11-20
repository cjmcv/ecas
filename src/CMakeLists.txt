cmake_minimum_required(VERSION 3.2)

# Include
SET(INCLUDE_DIR
    ${CMAKE_CURRENT_LIST_DIR}/core
    ${CMAKE_CURRENT_LIST_DIR}/util
)
message(STATUS "include: ${INCLUDE_DIR}")
include_directories(${INCLUDE_DIR})

# Source
file(GLOB_RECURSE SRC_LIST 
    "${CMAKE_CURRENT_LIST_DIR}/core/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/util/*.cpp"
    # Include
    "${INCLUDE_DIR}/*.hpp"
)
message(STATUS "src: ${SRC_LIST}")

set(DLAS_OBJECTS_TO_LINK "")

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/kernel/generic)
list(APPEND DLAS_OBJECTS_TO_LINK $<TARGET_OBJECTS:DLAS_GENERIC>)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/kernel/vulkan)
list(APPEND DLAS_OBJECTS_TO_LINK $<TARGET_OBJECTS:DLAS_VULKAN>)


add_library(${PROJECT_NAME} STATIC ${DLAS_OBJECTS_TO_LINK} ${SRC_LIST})  #SHARED
target_link_libraries(${PROJECT_NAME} -lvulkan)

set_target_properties(${PROJECT_NAME} PROPERTIES output_name "dlas") 

# Set output path
set(LIBRARY_OUTPUT_PATH ${OUTPUT_DIR})
message(STATUS "LIBRARY_OUTPUT_PATH: ${LIBRARY_OUTPUT_PATH}")