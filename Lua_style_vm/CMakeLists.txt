

file(GLOB_RECURSE SRC_RUNTIME
        runtime/*.h
        runtime/*.hpp
        runtime/*.c
        runtime/*.cpp
    )

file(GLOB_RECURSE SRC_SRC
        test/*.h
        test/*.hpp
        test/*.c
        test/*.cpp
    )

include_directories(${CMAKE_SOURCE_DIR}/)

add_executable(testLuaStyleVM
    ${SRC_RUNTIME}
    ${SRC_SRC}
    )
