cmake_minimum_required(VERSION 3.20)
project(glmp)

set(CMAKE_CXX_STANDARD 17)

add_library(glmp glmp/src/library.cc glmp/include/threadpool.hpp glmp/src/pipeline.cc glmp/include/pipeline.h glmp/include/concurrentqueue.hpp glmp/include/ringqueue.hpp)

add_executable(test test/test.cc)

include_directories(/usr/local/include thirdparty/GLFW/include thirdparty/FFMPEG/include glmp)

link_directories(thirdparty/GLFW/lib thirdparty/FFMPEG/lib)