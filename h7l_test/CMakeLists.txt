

file(GLOB_RECURSE SRC_H7_TEST
        src/*.h
        src/*.hpp
        src/*.c
        src/*.cpp
    )

add_executable(test_h7l
    ${SRC_H7_TEST}
    )
