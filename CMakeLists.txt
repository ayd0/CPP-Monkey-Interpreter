cmake_minimum_required(VERSION 3.10)

project(Interpreter)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(a.out ${SOURCES})

# Uncomment to enable tracing
# add_definitions(-DENABLE_TRACING)
