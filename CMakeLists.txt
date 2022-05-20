cmake_minimum_required(VERSION 3.20)
project(glmp)

set(CMAKE_CXX_STANDARD 17)

if(APPLE)
    add_compile_options(-fno-pie)
endif()

add_subdirectory(glmp)
add_subdirectory(test)

add_dependencies(test pipeline)