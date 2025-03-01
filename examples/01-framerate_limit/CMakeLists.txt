cmake_minimum_required(VERSION 3.8)

project(example)
set(PYBIND11_FINDPYTHON ON)
# pip install "pybind11[global]"
# python -c "import pybind11; print(pybind11.__path__)"
set(pybind11_DIR "C:\\Users\\dimweb\\AppData\\Local\\Programs\\Python\\Python311-32\\Lib\\site-packages\\pybind11\\share\\cmake\\pybind11")

# set(pybind11 "C:\\Users\\dimweb\\.pyenv\\pyenv-win\\versions\\3.10.6\\lib\\site-packages\\pybind11\\\\share\\cmake\\pybind11")
find_package(pybind11 REQUIRED)

add_executable(example main.cpp)
target_link_libraries(example PRIVATE pybind11::embed)

add_custom_target(
    copy_file_after_build
    ALL
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/calc.py
        ${CMAKE_BINARY_DIR}/
    DEPENDS ${CMAKE_SOURCE_DIR}/calc.py  # Depend on the source file
)