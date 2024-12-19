cmake_minimum_required(VERSION 3.5)

project(liblg LANGUAGES C CXX)
add_compile_options(-Wall -Werror)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -std=c11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

file(GLOB_RECURSE src src/lg/*.c)

add_library(lib_static STATIC ${src})
set_target_properties(lib_static PROPERTIES OUTPUT_NAME lg)
target_include_directories(lib_static PUBLIC src/)

add_executable(fibrec ${src} src/fibrec.c)
target_compile_options(fibrec PUBLIC -O2 -g)
target_include_directories(fibrec PUBLIC src/)

add_executable(lgasm ${src} src/lgasm.cpp)
target_compile_options(lgasm PUBLIC -O2 -g)
target_include_directories(lgasm PUBLIC src/)

file(GLOB inc src/lg/*.h)
install(FILES ${inc} DESTINATION include/lg)
install(FILES build/liblg.a DESTINATION lib)
install(FILES build/lgasm DESTINATION bin PERMISSIONS WORLD_EXECUTE)
