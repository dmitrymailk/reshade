cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 17) # Or your desired standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(example)

add_library(example SHARED main.cpp)

set(RESHADE_INCLUDE_DIR "F:\\project\\programming\\reshade\\include")
include_directories(${RESHADE_INCLUDE_DIR})

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(ADDON_EXTENSION ".addon32")
else()
	set(ADDON_EXTENSION ".addon")
endif()

set(CMAKE_SHARED_LIBRARY_SUFFIX ${ADDON_EXTENSION})