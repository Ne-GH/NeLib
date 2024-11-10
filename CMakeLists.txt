cmake_minimum_required(VERSION 3.29)
project(NeLib)

# set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(VERSION "1.0.0")
set(CMAKE_CXX_STANDARD 23)
find_package(OpenCV REQUIRED)

add_library(NeLib STATIC )

file(GLOB module_files "*.cppm")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/NeLibConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/NeLibConfig.cmake"
    @ONLY
)
target_sources(NeLib
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    ${module_files}
)

target_compile_features(NeLib PUBLIC cxx_std_23)

install(TARGETS  ${PROJECT_NAME}  DESTINATION /usr/lib EXPORT ${PROJECT_NAME} FILE_SET cxx_modules)
install(EXPORT ${PROJECT_NAME} DESTINATION /usr/lib/cmake/NeLib/)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/NeLibConfig.cmake"
        DESTINATION "${test_INSTALL_DIR}/lib/cmake/NeLib/")
