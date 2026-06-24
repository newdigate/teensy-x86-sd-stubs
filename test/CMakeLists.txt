cmake_minimum_required(VERSION 3.5)
project(tests CXX C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)


set (Boost_USE_STATIC_LIBS OFF)
find_package (Boost REQUIRED COMPONENTS unit_test_framework)
include_directories (${Boost_INCLUDE_DIRS})

include_directories(../src)

# All test translation units are picked up automatically. Drop a new
# test_<area>.cpp file in this directory and it is compiled into the single
# `tests` executable -- no edits to this file required. See TESTING.md.
# CONFIGURE_DEPENDS makes CMake re-glob on rebuild so CI fresh builds always
# see new files.
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp")

add_executable(tests ${TEST_SOURCES})

target_link_libraries(tests teensy_x86_sd_stubs)
target_link_libraries(tests teensy_x86_stubs)
target_link_libraries(tests ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})