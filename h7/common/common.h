#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
}
#endif

#include <string>
#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>
#include <future>
#include "common/common_base.h"

namespace h7l {
using CString = const std::string&;
using String = std::string;
template<typename T>
using List = std::vector<T>;

template<typename T>
using CList = const std::vector<T>&;
typedef unsigned int U32;
}
#define HMIN(a, b) (a < b ? a : b)
#define HMAX(a, b) (a > b ? a : b)

#if defined(_WIN32)|| defined(WIN32)
    #include <windows.h>
#define SLEEP(x) Sleep(x) //msec
#else
    #include <unistd.h>
#define SLEEP(x) usleep(x*1000)
#endif

#define DEFAULT_SEQ '\t'

#define DELETE_P(p) if(p){delete p; p = NULL;}
#define COPY_MEMBER(src, dst, name) dst.name = src.name

#define FUNC_PLACEHOLDERS_1 std::placeholders::_1
#define FUNC_PLACEHOLDERS_2 FUNC_PLACEHOLDERS_1,std::placeholders::_2
#define FUNC_PLACEHOLDERS_3 FUNC_PLACEHOLDERS_2,std::placeholders::_3
#define FUNC_PLACEHOLDERS_4 FUNC_PLACEHOLDERS_3,std::placeholders::_4
#define FUNC_PLACEHOLDERS_5 FUNC_PLACEHOLDERS_4,std::placeholders::_5
#define FUNC_PLACEHOLDERS_6 FUNC_PLACEHOLDERS_5,std::placeholders::_6
#define FUNC_PLACEHOLDERS_7 FUNC_PLACEHOLDERS_6,std::placeholders::_7
#define FUNC_PLACEHOLDERS_8 FUNC_PLACEHOLDERS_7,std::placeholders::_8
#define FUNC_PLACEHOLDERS_9 FUNC_PLACEHOLDERS_8,std::placeholders::_9

#define FUNC_SHARED_PTR(func_expre) std::shared_ptr<std::packaged_task<func_expre>>
#define FUNC_MAKE_SHARED_PTR(func, func_expre, place_holders) \
std::make_shared<std::packaged_task<func_expre>>(std::bind(func, place_holders))

#define FUNC_MAKE_SHARED_PTR_0(func_expre, func) std::make_shared<std::packaged_task<func_expre>>(std::bind(func))
#define FUNC_MAKE_SHARED_PTR_1(func_expre, func) FUNC_MAKE_SHARED_PTR(func, func_expre, FUNC_PLACEHOLDERS_1)
#define FUNC_MAKE_SHARED_PTR_2(func_expre, func) FUNC_MAKE_SHARED_PTR(func, func_expre, FUNC_PLACEHOLDERS_2)
#define FUNC_MAKE_SHARED_PTR_3(func_expre, func) FUNC_MAKE_SHARED_PTR(func, func_expre, FUNC_PLACEHOLDERS_3)
#define FUNC_MAKE_SHARED_PTR_4(func_expre, func) FUNC_MAKE_SHARED_PTR(func, func_expre, FUNC_PLACEHOLDERS_4)
#define FUNC_MAKE_SHARED_PTR_5(func_expre, func) FUNC_MAKE_SHARED_PTR(func, func_expre, FUNC_PLACEHOLDERS_5)
#define FUNC_MAKE_SHARED_PTR_6(func_expre, func) FUNC_MAKE_SHARED_PTR(func, func_expre, FUNC_PLACEHOLDERS_6)
#define FUNC_MAKE_SHARED_PTR_7(func_expre, func) FUNC_MAKE_SHARED_PTR(func, func_expre, FUNC_PLACEHOLDERS_7)
#define FUNC_MAKE_SHARED_PTR_8(func_expre, func) FUNC_MAKE_SHARED_PTR(func, func_expre, FUNC_PLACEHOLDERS_8)
#define FUNC_MAKE_SHARED_PTR_9(func_expre, func) FUNC_MAKE_SHARED_PTR(func, func_expre, FUNC_PLACEHOLDERS_9)

#define PRINTLN(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define PRINTERR(fmt, ...) fprintf(stderr,fmt, ##__VA_ARGS__)

#define LOGI(fmt, ...) PRINTLN(fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) PRINTERR(fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) PRINTERR(fmt, ##__VA_ARGS__)

#define CMP_AESC(x, y) (x > y ? 1: (x != y ? -1 : 0))
#define CMP_DESC(x, y) (x > y ? -1: (x != y ? 1 : 0))

#define CPP_FOREACH(coll, code)\
do{\
    auto end = coll.end();\
    for(auto it = coll.begin() ; it != end ; it++){\
        code;\
    }\
}while(0);

#define CPP_FOREACH_END(coll, code)\
do{\
    auto end = coll.end();\
    bool isEnd;\
    for(auto it = coll.begin() ; it != end ; it++){\
        isEnd = std::next(it) == end;\
        code;\
    }\
}while(0);

#define H7_DISALLOW_COPY_AND_MOVE(TypeName)                                                                           \
  TypeName(const TypeName &) = delete;                                                                                 \
  TypeName & operator=(const TypeName &) = delete;                                                                     \
  TypeName(TypeName &&) = delete;                                                                                      \
  TypeName & operator=(TypeName &&) = delete



