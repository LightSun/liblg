cmake_minimum_required(VERSION 3.10)

project(h7l VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if (MSVC)
    add_compile_definitions(USE_MSC_ATOMICS)
else(MSVC)
    add_compile_definitions(USE_C11_ATOMICS)
endif(MSVC)

#include_directories(./)
#include_directories(h7/)
include_directories(Lua_style_vm/)

#add_subdirectory(h7)
#add_subdirectory(h7l_test)
add_subdirectory(Lua_style_vm)
