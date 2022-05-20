cmake_minimum_required(VERSION 3.20)
project(glmp)

set(CMAKE_CXX_STANDARD 17)

include_directories(/usr/local/include thirdparty/cocoyaxi/include glmp)

link_directories(thirdparty/cocoyaxi/lib)

add_library(glmp glmp/src/library.cc glmp/include/threadpool.hpp glmp/src/pipeline.cc glmp/include/pipeline.h glmp/include/concurrentqueue.hpp glmp/include/ringqueue.hpp)

add_executable(test test/test.cc)

target_link_libraries(test co)

add_compile_options(-stdlib=libc++)