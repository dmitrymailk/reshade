cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 17) # Or your desired standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(example)

# pip install "pybind11[global]"
# python -c "import pybind11; print(pybind11.__path__)"
# set(pybind11_DIR "C:\\Users\\dimweb\\AppData\\Local\\Programs\\Python\\Python311\\Lib\\site-packages\\pybind11\\share\\cmake\\pybind11")
# find_package(pybind11 REQUIRED)

add_library(example SHARED main.cpp)

set(RESHADE_INCLUDE_DIR "D:\\programming\\auto_remaster\\reshade\\include")
include_directories(${RESHADE_INCLUDE_DIR})
set(GAME_ROOT "D:\\Games\\NeedforSpeedMostWantedOriginal")
# add_custom_target(
# 	copy_file_after_build
# 	ALL
# 	COMMAND ${CMAKE_COMMAND} -E copy
# 	${CMAKE_SOURCE_DIR}/calc.py
# 	${GAME_ROOT}/
# 	DEPENDS ${CMAKE_SOURCE_DIR}/calc.py # Depend on the source file
# )

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(ADDON_EXTENSION ".addon32")
else()
	set(ADDON_EXTENSION ".addon")
endif()

set(CMAKE_SHARED_LIBRARY_SUFFIX ${ADDON_EXTENSION})