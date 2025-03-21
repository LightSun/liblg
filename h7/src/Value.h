#pragma once

#include "common/c_common.h"

namespace h7l {

struct Type;

struct Value{
    Type* type {nullptr};

    union {
        int64_t as_int;
        void *as_ptr;
    };

    void init(Type* type){
        this->type = type;
    }
};
}
