cmake_minimum_required(VERSION 3.22)
project(PathFinding VERSION 0.1.0.0 DESCRIPTION "Path finding" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

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
set(TARGET_NAME, path-finding)
set(SOURCES 
        src/collision.cpp 
        src/collision.h 
        src/color.cpp 
        src/color.h 
        src/graph.cpp 
        src/graph.h 
        src/graphviz.cpp 
        src/graphviz.h 
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

# Strict warning level
if(MSVC)
  target_compile_options(path-finding PRIVATE /W4 /WX)
else()
  #target_compile_options(path-finding PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()



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
        src/color.test.cpp
        src/graph.test.cpp
        src/graphviz.test.cpp
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

# Strict warning level
if(MSVC)
  target_compile_options(path-finding-test PRIVATE /W4 /WX)
else()
  #target_compile_options(path-finding-test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()



include(GoogleTest)
gtest_discover_tests(path-finding-test)