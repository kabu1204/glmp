file(GLOB_RECURSE PPL_SRC_FILES ${PROJECT_SOURCE_DIR}/glmp/**/*.h
                            ${PROJECT_SOURCE_DIR}/glmp/**/*.hpp
                        ${PROJECT_SOURCE_DIR}/glmp/**/*.cc)

message(${PPL_SRC_FILES})

include_directories(${PROJECT_SOURCE_DIR}/glmp)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/cocoyaxi/include)

add_library(pipeline ${PPL_SRC_FILES})

set_target_properties(pipeline PROPERTIES LINKER_LANGUAGE CXX)