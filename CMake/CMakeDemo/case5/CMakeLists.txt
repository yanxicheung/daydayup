cmake_minimum_required (VERSION 2.8)

project (Demo2)

if(DEFINED USE_MYMATH)
    message("USE_MYMATH is defined")
    add_definitions(-DUSE_MYMATH)
else()
    message("USE_MYMATH is not defined")
endif()

aux_source_directory(. DIR_SRCS)
add_executable(Demo ${DIR_SRCS})
