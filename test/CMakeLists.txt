cmake_minimum_required(VERSION 3.8)

project(path_tracer_test
    VERSION 2.0
    DESCRIPTION "C++ path tracer tests"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)  

find_package(Catch2 3 REQUIRED)

file(GLOB SOURCES
    ../src/*.cpp
)

add_executable(vec3_test vec3_test.cpp ${SOURCES})
target_include_directories(vec3_test PUBLIC ../include)
target_compile_features(vec3_test PUBLIC cxx_std_20)
target_link_libraries(vec3_test Catch2::Catch2WithMain)
set_target_properties(vec3_test PROPERTIES COMPILE_FLAGS "-g -Wall -Wextra -Wpedantic")

add_executable(random_test random_test.cpp ${SOURCES})
target_include_directories(random_test PUBLIC ../include)
target_compile_features(random_test PUBLIC cxx_std_20)
target_link_libraries(random_test Catch2::Catch2WithMain)
set_target_properties(random_test PROPERTIES COMPILE_FLAGS "-g -Wall -Wextra -Wpedantic -Wno-unused-but-set-variable")