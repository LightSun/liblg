cmake_minimum_required(VERSION 3.10)

project(h7l VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#include_directories(./)
#include_directories(h7/)

#add_subdirectory(h7)
#add_subdirectory(h7l_test)
add_subdirectory(Lua_style_vm)
