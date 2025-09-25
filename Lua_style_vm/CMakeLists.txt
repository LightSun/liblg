

file(GLOB_RECURSE SRC_LANG
        *.h
        *.hpp
        *.c
        *.cpp
    )

add_executable(testLuaStyleVM
    ${SRC_LANG}
    )
