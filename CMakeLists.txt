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
include_directories(${CMAKE_SOURCE_DIR}/vendor/nlohmann_json/include)
include_directories(${CMAKE_BINARY_DIR}/vendor/nlohmann_json/include) 
include_directories(${CMAKE_SOURCE_DIR}/vendor/stb)
include_directories(${CMAKE_BINARY_DIR}/vendor/stb) 

add_subdirectory(vendor/SDL)
add_subdirectory(vendor/nlohmann_json)

file(GLOB SOURCES
    "src/*.cpp"
    "main.cpp"
)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

add_library(${CMAKE_PROJECT_NAME} STATIC ${SOURCES})  

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE 
    vendor/SDL/include
    ${CMAKE_BINARY_DIR}/vendor/SDL/include
    vendor/stb 
    ${CMAKE_BINARY_DIR}/vendor/stb
)

set_source_files_properties(src/app.cpp PROPERTIES COMPILE_OPTIONS -Wno-missing-field-initializers)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE SDL3::SDL3)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

target_compile_features(${CMAKE_PROJECT_NAME} PUBLIC cxx_std_20)

add_executable(${CMAKE_PROJECT_NAME}_app main.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME}_app PRIVATE ${CMAKE_PROJECT_NAME})

set(INIT_FOLDER ${CMAKE_SOURCE_DIR}/init)
set(BUILD_INIT_FOLDER ${CMAKE_BINARY_DIR}/init)

add_custom_target(copy_init ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${INIT_FOLDER} ${BUILD_INIT_FOLDER}
    COMMENT "Copying init folder to build directory"
)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_PROFILE "-g -O3")

add_subdirectory(test)
