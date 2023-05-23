cmake_minimum_required(VERSION 3.2)
project(LinuxCommand)

set(CMAKE_CXX_STANDARD  17)





add_executable(linuxcommand demo.cpp ${SOURCE_FILES} ${SOURCES} myls/myls_cpp.h mypwd/mypwd_cpp.h)