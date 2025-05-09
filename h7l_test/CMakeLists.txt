

file(GLOB_RECURSE SRC_H7_TEST
        src/*.h
        src/*.hpp
        src/*.c
        src/*.cpp
    )

add_executable(test_h7l
    ${SRC_H7_TEST}
    )

target_link_libraries(test_h7l PUBLIC h7l)
