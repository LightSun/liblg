

file(GLOB_RECURSE SRC_RUNTIME
        runtime/*.h
        runtime/*.hpp
        runtime/*.c
        runtime/*.cpp
    )

file(GLOB_RECURSE SRC_SRC
        src/*.h
        src/*.hpp
        src/*.c
        src/*.cpp
    )

add_executable(testLuaStyleVM
    ${SRC_RUNTIME}
    ${SRC_SRC}
    )
