cmake_minimum_required(VERSION 3.2)

# Define project name
project(ecas)
set(PROJECT_NAME ecas)

option(ECAS_UNIT_TESTS "Build with unit tests" ON) # googletest 1.13 requires C++14
option(ECAS_GPU_USE_VULKAN "GPU acceleration using vulkan" ON)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX14)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
elseif(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler.")
endif()

# Release by default
set(CMAKE_BUILD_TYPE "Release")  # Debug

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC") # Set for .so
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g") 
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)

if(ECAS_UNIT_TESTS)
    set(GOOGLETEST_VERSION 1.13.0) # C++14
    set(GTEST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/googletest/googletest")
    set(GTEST_INCLUDE_DIR ${GTEST_ROOT}/include)
    set(GTEST_LIBRARIES gtest gtest_main)
    set(GTEST_MAIN_LIBRARY gtest_main)
    set(GTEST_LIBRARY gtest)
    add_subdirectory(${GTEST_ROOT})
endif()

############################################
# source
set(ECAS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(src)

############################################
# examples
add_subdirectory(examples)

############################################
# tests
if(ECAS_UNIT_TESTS)
    add_subdirectory(tests)
endif()