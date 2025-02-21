cmake_minimum_required(VERSION 3.8)

project(path_tracer
    VERSION 1.0
    DESCRIPTION "C++ path tracer tests"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)  

find_package(Catch2 3 REQUIRED)

file(GLOB SOURCES
    ../src/*.cpp
)

add_executable(thread_safe_queue_test thread_safe_queue_test.cpp ${SOURCES})
target_include_directories(thread_safe_queue_test PUBLIC ../include)
target_compile_features(thread_safe_queue_test PUBLIC cxx_std_20)
target_link_libraries(thread_safe_queue_test Catch2::Catch2WithMain)

set_target_properties(thread_safe_queue_test PROPERTIES COMPILE_FLAGS "-g -Wall -Wextra -Wpedantic")