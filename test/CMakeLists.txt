cmake_minimum_required(VERSION 3.5)
project(tests CXX C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)


set (Boost_USE_STATIC_LIBS OFF)
find_package (Boost REQUIRED COMPONENTS unit_test_framework)
include_directories (${Boost_INCLUDE_DIRS})

include_directories(../src)

add_executable(tests
        test_write.cpp)

target_link_libraries(tests teensy_x86_sd_stubs)
target_link_libraries(tests teensy_x86_stubs)
target_link_libraries(tests ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})