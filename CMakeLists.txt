cmake_minimum_required(VERSION 3.20)

project(EasySTl)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

set(BUILD_SHARED_LIBS ON)
add_executable(main src/main.cpp)
target_include_directories(main PRIVATE include)
add_definitions(-DHEADER_STYLE)

# add_subdirectory(3rdlib)

enable_testing()
include(GoogleTest)
add_subdirectory(test)
