cmake_minimum_required(VERSION 3.2)

# Define project name
project(ecas)
set(PROJECT_NAME ecas)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

option(ECAS_GPU_USE_VULKAN "GPU acceleration using vulkan" OFF)

# Release by default
set(CMAKE_BUILD_TYPE "Release")  # Debug

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC") # Set for .so
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g") 
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)

############################################
# ecas source
set(ECAS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(src)

############################################
# tests
#add_subdirectory(tests)

############################################
# examples
add_subdirectory(examples)

