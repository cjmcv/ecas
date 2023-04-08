# Define project name
project(unit_tests)
set(PROJECT_NAME unit_tests)

# Include
SET(INCLUDE_LIST 
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/../src
    ${PROJECT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_LIST_DIR}
)
message(STATUS "tests include: ${INCLUDE_LIST}")
include_directories(${INCLUDE_LIST})

# Source
file(GLOB_RECURSE TESTS_SRC_LIST 
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
)
message(STATUS "tests src: ${TESTS_SRC_LIST}")

# Build
add_executable(${PROJECT_NAME} ${TESTS_SRC_LIST})

# Depends on project ecas.
target_link_libraries(${PROJECT_NAME} ecas ${GTEST_LIBRARY})

# Set output path
set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR})