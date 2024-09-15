cmake_minimum_required(VERSION 3.5)
project(teensy_x86_sd_stubs C CXX)
set(teensy_x86_sd_stubs_VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 14)

find_package(teensy_x86_stubs)
include_directories(${teensy_x86_stubs_INCLUDE_DIR})

set(SOURCE_FILES
		File.cpp
		SD.cpp
		InMemoryFile.cpp
		LinuxFile.cpp
		utility/Sd2Card.cpp
		utility/SdFile.cpp
		utility/SdVolume.cpp)

set(HEADER_FILES
		SD.h)

set(UTILITY_HEADER_FILES
		utility/Sd2Card.h
		utility/Sd2PinMap.h
		utility/SdFat.h
		utility/SdFatUtil.h
		utility/FatStructs.h
		utility/SdFatmainpage.h
		utility/SdInfo.h)

add_library(teensy_x86_sd_stubs SHARED STATIC ${HEADER_FILES} ${UTILITY_HEADER_FILES} ${SOURCE_FILES})