

file(GLOB_RECURSE SRC_LANG
        *.h
        *.hpp
        *.c
        *.cpp
    )

file(GLOB_RECURSE SRC_LUA_STYLE_VM
        *.h
        *.hpp
        *.c
        *.cpp
    )

add_executable(testLuaStyleVM
    ${SRC_LANG}
    ${SRC_LUA_STYLE_VM}
    )
