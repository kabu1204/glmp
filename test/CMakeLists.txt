AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/test SRC_FILES)

include_directories(${PROJECT_SOURCE_DIR}/glmp)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/cocoyaxi/include)

link_directories(${PROJECT_SOURCE_DIR}/thirdparty/cocoyaxi/lib)

add_executable(test ${SRC_FILES})
set_target_properties(test PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(test pipeline co)