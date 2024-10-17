cmake_minimum_required(VERSION 3.29)
project(NeLib)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenCV REQUIRED)

add_library(NeLib STATIC )

file(GLOB module_files "*.cppm")

target_sources(NeLib
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    ${module_files}
)
