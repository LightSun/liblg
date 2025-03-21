cmake_minimum_required(VERSION 3.10)

project(h7l VERSION 1.0 LANGUAGES C CXX)

include_directories(./)

file(GLOB_RECURSE SRC_H7
        *.h
        *.c
        *.cpp
    )
add_library(libh7l
    ${SRC_H7}
    )


