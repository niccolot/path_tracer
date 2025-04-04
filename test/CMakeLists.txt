# Ensure Catch2 is found in the vendor folder
find_package(Catch2 REQUIRED)

# Add each test file as a separate executable
file(GLOB TEST_SOURCES "*.cpp")

foreach(test_file ${TEST_SOURCES})
    # Get the name of the test file without the extension
    get_filename_component(test_name ${test_file} NAME_WE)

    # Add an executable for each test file
    add_executable(${test_name} ${test_file})

    # Link Catch2 with the test executable
    target_link_libraries(${test_name} PRIVATE Catch2::Catch2WithMain)

    # Link any other libraries your tests need (e.g., your main project)
    target_link_libraries(${test_name} PRIVATE ${CMAKE_PROJECT_NAME})

    # Optionally, include directories for the test
    target_include_directories(${test_name} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_include_directories(${test_name} PRIVATE ${CMAKE_BINARY_DIR}/vendor/stb)
    target_include_directories(${test_name} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/SDL/include)
    target_include_directories(${test_name} PRIVATE ${CMAKE_BINARY_DIR}/vendor/SDL/include)
    target_include_directories(${test_name} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/nlohmann_json/include)

    # If you want to set specific compile flags for tests, you can do it here.
    target_compile_options(${test_name} PRIVATE -Wall -Wextra -Wpedantic)
endforeach()

# Enable testing and add all tests to the test suite
include(CTest)
foreach(test_file ${TEST_SOURCES})
    get_filename_component(test_name ${test_file} NAME_WE)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

