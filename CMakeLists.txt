cmake_minimum_required(VERSION 3.22)
project(PathFinding VERSION 0.1.0.0 DESCRIPTION "Path finding" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# setup Conan dependencies
set(CONAN_DIR .conan)
include(${CONAN_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

#
# Import Boost
#
find_package(Boost 1.80.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

#
# Define application target
#
add_executable(path-finding src/main.cpp src/graph.cpp)
configure_file(src/version.h.in version.h)
target_include_directories(path-finding PUBLIC
        "${PROJECT_BINARY_DIR}"
        )

#
# Setup GoogleTest
#
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

#
# Define test target
#
enable_testing()

add_executable(
        path-finding-test
        src/main.test.cpp
)
target_link_libraries(
        path-finding-test
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(path-finding-test)