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

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

add_subdirectory(vendor/SDL)
add_subdirectory(vendor/nlohmann_json)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})  

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE 
    vendor/SDL/include
    ${CMAKE_BINARY_DIR}/vendor/SDL/include 
)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE SDL3::SDL3)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

target_compile_features(${CMAKE_PROJECT_NAME} PUBLIC cxx_std_20)

set(INIT_FOLDER ${CMAKE_SOURCE_DIR}/init)
set(BUILD_INIT_FOLDER ${CMAKE_BINARY_DIR}/init)
set(TIMESTAMP_FILE ${CMAKE_BINARY_DIR}/init_copy_timestamp)

file(GLOB_RECURSE INIT_FILES ${INIT_FOLDER}/*)

add_custom_command(
    OUTPUT ${TIMESTAMP_FILE}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${INIT_FOLDER} ${BUILD_INIT_FOLDER}
    COMMAND ${CMAKE_COMMAND} -E touch ${TIMESTAMP_FILE}
    DEPENDS ${INIT_FOLDER} ${INIT_FILES}
    COMMENT "Copying init folder to build directory if modified"
)

add_custom_target(copy_init ALL DEPENDS ${TIMESTAMP_FILE})

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_PROFILE "-g -O3")