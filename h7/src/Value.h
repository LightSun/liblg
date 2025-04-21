#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "src/Type.h"

namespace h7l {

struct IObject{
    virtual ~IObject(){};
    virtual void unref() = 0;
};

struct Value {
    Type *type {nullptr};
    union {
        int64_t as_long;
        char as_char;
        unsigned char as_uchar;
        short as_short;
        int as_int;
        unsigned int as_uint;
        float as_float;
        double as_double;
        void *as_ptr;
    };
    void init(Type *type){
        this->type = type;
    }
    void deinit(){
        if(!type->isPrimetiveType()){
            auto obj = (IObject*)as_ptr;
            obj->unref();
        }
    }
    const char* typeStr(){
        return type->id.data();
    }
    void setPrimitiveValue(Type *type, char* ptr);
};

}

