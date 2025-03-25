#pragma once

#include <vector>
#include "utils/h_atomic.h"

namespace h7l {

typedef unsigned int U32;

struct Type;

struct MemoryBlock{
    void* data {nullptr};
    U32 len;
    U32 alignLen;
};

struct FieldInfo{
    Type* type {nullptr};
    U32 offset;
};

struct ClassInfo{
    Type* type {nullptr};
    std::vector<FieldInfo> fields;
    U32 structSize;

    ClassInfo(const std::vector<FieldInfo>& fields);
};

struct Object
{
    MemoryBlock data;
    ClassInfo* clsInfo {nullptr};
    Object* super {nullptr};
    volatile int ref {1};
    U32 flags {0};

    void ref(){
        h_atomic_add(&ref, 1);
    }
    int unref(){
        return h_atomic_add(&ref, -1);
    }
};

}
