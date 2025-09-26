#pragma once

#include <atomic>
#include <string>
#include "runtime/common_base.h"

namespace h7l { namespace runtime {

#ifdef _WIN32
    typedef long long Long;
typedef unsigned long long ULong;
#else
    typedef long int Long;
typedef unsigned long int ULong;
#endif

using String = std::string;
using CString = const std::string&;

enum Type{
    kType_NONE = -1,
    kType_S8 ,
    kType_U8,
    kType_S16 ,
    kType_U16 ,
    kType_S32,
    kType_U32 ,
    kType_S64,
    kType_U64,
    kType_BOOL,
    kType_FLOAT,
    kType_DOUBLE,
    kType_VOID,
    kType_NULL,
    kType_TABLE,
    kType_CLOSURE,
    kType_STRING,
    kType_CFUNCTION,
    };

//string
//struct Table
//struct Closure
//string CFunction
typedef union TypeValue{
    char s8;
    unsigned char u8;
    short s16;
    unsigned short u16;
    int s32;
    unsigned int u32;
    Long s64;
    ULong u64;
    bool b;
    float f32;
    double f64;
    void* ptr;
}TypeValue;

struct IObjectType
{
    std::atomic_int refCnt {1};

    virtual ~IObjectType(){
        unref();
    }

    void ref(){
        refCnt.fetch_add(1, std::memory_order_relaxed);
    }
    void unref(){
        if(refCnt.fetch_add(-1, std::memory_order_relaxed) == 1){
            delete this;
        }
    }
    virtual void printTo(std::ostream& ss) = 0;
};

template<typename T>
struct BaseObjectType: public IObjectType{

    void print(){
        std::cout << *this << std::endl;
    }
    friend std::ostream& operator<<(std::ostream& os, const T& bni){
        ((T&)bni).printTo(os);
        return os;
    }
};

}}
