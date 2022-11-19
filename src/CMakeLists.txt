cmake_minimum_required(VERSION 3.2)

# Include
# SET(INCLUDE_DIR
#     ${CUX_INCLUDE_DIR}
# )
# message(STATUS "include: ${INCLUDE_DIR}")
# include_directories(${INCLUDE_DIR})

# # Source
# file(GLOB_RECURSE SRC_LIST 
#     "${PROJECT_SOURCE_DIR}/*.cpp"
#     "${PROJECT_SOURCE_DIR}/common/*.cpp"
#     # Include
#     "${INCLUDE_DIR}/*.h"
# )
# message(STATUS "src: ${SRC_LIST}")

set(DLAS_OBJECTS_TO_LINK "")

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/kernel/generic)
list(APPEND DLAS_OBJECTS_TO_LINK $<TARGET_OBJECTS:DLAS_GENERIC>)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/kernel/vulkan)
list(APPEND DLAS_OBJECTS_TO_LINK $<TARGET_OBJECTS:DLAS_VULKAN>)


add_library(${PROJECT_NAME} STATIC ${DLAS_OBJECTS_TO_LINK})  #SHARED${SRC_LIST}
target_link_libraries(${PROJECT_NAME} -lvulkan)

set_target_properties(${PROJECT_NAME} PROPERTIES output_name "dlas") 

# Set output path
set(LIBRARY_OUTPUT_PATH ${OUTPUT_DIR})
message(STATUS "LIBRARY_OUTPUT_PATH: ${LIBRARY_OUTPUT_PATH}")