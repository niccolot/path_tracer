cmake_minimum_required(VERSION 3.16)

project(path_tracer
    VERSION 2.0
    DESCRIPTION "C++ path tracer"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)  

include_directories(include)

include_directories(${CMAKE_SOURCE_DIR}/vendor/SDL/include)
include_directories(${CMAKE_BINARY_DIR}/vendor/SDL/include) 

file(GLOB SOURCES
    "src/*.cpp"
    "main.cpp"
)

set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)

add_subdirectory(vendor/SDL)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})  

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE 
    vendor/SDL/include
    ${CMAKE_BINARY_DIR}/vendor/SDL/include 
)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE SDL3::SDL3)

target_compile_features(${CMAKE_PROJECT_NAME} PUBLIC cxx_std_20)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_PROFILE "-g -O2")