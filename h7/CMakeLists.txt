#cmake_minimum_required(VERSION 3.10)

#project(h7l VERSION 1.0 LANGUAGES C CXX)

#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(USE_C11_ATOMICS)

include_directories(./)

file(GLOB_RECURSE SRC_H7
        *.h
        *.hpp
        *.c
        *.cpp
    )
add_library(h7l
    ${SRC_H7}
    )

target_compile_definitions(h7l PUBLIC H7L_DBG_ShareData_DELETE)
###


