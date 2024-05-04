cmake_minimum_required(VERSION 3.22)
project(PathFinding VERSION 0.1.0.0 DESCRIPTION "Path finding" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#
# Import Boost
#
find_package(Boost 1.85.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

#
# Define application target
#
set(SOURCES src/graph.cpp  src/graph.h)
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
        ${SOURCES}
)
target_link_libraries(
        path-finding-test
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(path-finding-test)