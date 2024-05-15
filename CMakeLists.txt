cmake_minimum_required(VERSION 3.22)
project(PathFinding VERSION 0.1.0.0 DESCRIPTION "Path finding" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Get the project root directory
get_filename_component(PROJECT_ROOT_DIR ${CMAKE_HOME_DIRECTORY} ABSOLUTE)

# Add a preprocessor definition with the project root directory
add_definitions(-DPROJECT_ROOT_DIR="${PROJECT_ROOT_DIR}")

#
# Import Boost
#
find_package(Boost 1.85.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

#
# Define application target
#
set(SOURCES 
        src/collision.cpp 
        src/collision.h 
        src/graph.cpp 
        src/graph.h 
        src/runner.cpp 
        src/runner.h 
        src/scenario.cpp 
        src/scenario.h 
        src/simulation.cpp 
        src/simulation.h 
        src/strings.cpp 
        src/strings.h)
add_executable(path-finding src/main.cpp ${SOURCES})
configure_file(src/version.h.in version.h)
target_include_directories(path-finding PUBLIC
        "${PROJECT_BINARY_DIR}"
        )

#
# Define test target
#
find_package(GTest 1.14.0 REQUIRED)
include_directories(${GTest_INCLUDE_DIRS})

enable_testing()

add_executable(
        path-finding-test
        src/main.test.cpp
        src/collision.test.cpp
        src/graph.test.cpp
        src/runner.test.cpp
        src/scenario.test.cpp
        src/simulation.test.cpp
        src/strings.test.cpp
        ${SOURCES}
)
target_link_libraries(
        path-finding-test
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(path-finding-test)