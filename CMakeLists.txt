cmake_minimum_required(VERSION 3.8)

project(path_tracer
    VERSION 1.0
    DESCRIPTION "C++ path tracer"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)  

include_directories(include)

file(GLOB SOURCES
    "src/*.cpp"
    "main.cpp"
)

list(FILTER SOURCES EXCLUDE REGEX "src/material_old.cpp")

add_executable(path_tracer ${SOURCES})  

target_compile_features(path_tracer PUBLIC cxx_std_20)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_PROFILE "-g -O2")
