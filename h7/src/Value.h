#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "src/Type.h"

namespace h7l {

struct Value {
    Type *type {nullptr};

    union {
        int64_t as_long;
        char as_char;
        short as_short;
        int as_int;
        float as_float;
        double as_double;
        void *as_ptr;
    };
    void init(Type *type){
        this->type = type;
    }
    void deinit(){

    }
    const char* typeStr(){
        return type->id.data();
    }
};

}

