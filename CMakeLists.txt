cmake_minimum_required(VERSION 3.20)
project(glmp)

set(CMAKE_CXX_STANDARD 14)

add_library(glmp glmp/library.cc)

add_executable(test test/test.cc)

include_directories(/usr/local/include thirdparty/GLFW/include glmp)

link_directories(thirdparty/GLFW/lib)