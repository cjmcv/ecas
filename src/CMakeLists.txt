cmake_minimum_required(VERSION 3.2)

# Include
SET(INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}
)
message(STATUS "include: ${INCLUDE_DIR}")
include_directories(${INCLUDE_DIR})

# Source
file(GLOB_RECURSE SRC_LIST 
    "${CMAKE_CURRENT_LIST_DIR}/core/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/util/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/kernel/kernel_dispatcher.cpp"
    # Include
    "${INCLUDE_DIR}/*.hpp"
)
message(STATUS "src: ${SRC_LIST}")

set(ECAS_OBJECTS_TO_LINK "")
set(ECAS_LIBS_TO_LINK "")

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/kernel/generic)
list(APPEND ECAS_OBJECTS_TO_LINK $<TARGET_OBJECTS:ECAS_GENERIC>)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/kernel/x86)
list(APPEND ECAS_OBJECTS_TO_LINK $<TARGET_OBJECTS:ECAS_X86>)

if (ECAS_GPU_USE_VULKAN)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/kernel/vulkan)
    list(APPEND ECAS_OBJECTS_TO_LINK $<TARGET_OBJECTS:ECAS_VULKAN>)
    if (VULKAN_WIN_SDK) # for windows x86 sdk
        list(APPEND ECAS_LIBS_TO_LINK ${VULKAN_WIN_SDK}/Lib/vulkan-1.lib)
        message(STATUS "VULKAN_WIN_SDK: ${VULKAN_WIN_SDK}")
    else()
        list(APPEND ECAS_LIBS_TO_LINK -lvulkan)
    endif()
endif()

add_library(${PROJECT_NAME} STATIC ${ECAS_OBJECTS_TO_LINK} ${SRC_LIST})  #SHARED
target_link_libraries(${PROJECT_NAME} ${ECAS_LIBS_TO_LINK})
message(STATUS "ECAS_LIBS_TO_LINK: ${ECAS_LIBS_TO_LINK}")

set_target_properties(${PROJECT_NAME} PROPERTIES output_name "ecas") 

# Set output path
set(LIBRARY_OUTPUT_PATH ${OUTPUT_DIR})
message(STATUS "LIBRARY_OUTPUT_PATH: ${LIBRARY_OUTPUT_PATH}")