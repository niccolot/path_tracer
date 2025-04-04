find_package(Catch2 REQUIRED)

file(GLOB TEST_SOURCES "*.cpp")

foreach(test_file ${TEST_SOURCES})
    get_filename_component(test_name ${test_file} NAME_WE)

    add_executable(${test_name} ${test_file})

    target_link_libraries(${test_name} PRIVATE Catch2::Catch2WithMain)

    target_link_libraries(${test_name} PRIVATE ${CMAKE_PROJECT_NAME})

    target_include_directories(${test_name} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_include_directories(${test_name} PRIVATE ${CMAKE_BINARY_DIR}/vendor/stb)
    target_include_directories(${test_name} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/SDL/include)
    target_include_directories(${test_name} PRIVATE ${CMAKE_BINARY_DIR}/vendor/SDL/include)
    target_include_directories(${test_name} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/nlohmann_json/include)

    target_compile_options(${test_name} PRIVATE -Wall -Wextra -Wpedantic)
endforeach()

include(CTest)
foreach(test_file ${TEST_SOURCES})
    get_filename_component(test_name ${test_file} NAME_WE)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

