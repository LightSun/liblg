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
    IObject* objPtr {nullptr};
//    union {
//        int64_t as_long;
//        char as_char;
//        unsigned char as_uchar;
//        short as_short;
//        int as_int;
//        unsigned int as_uint;
//        float as_float;
//        double as_double;
//        void *as_ptr;
//    };

    Type* getType(){
//TODO
    }
    void deinit(){
        if(objPtr){
            objPtr->unref();
            objPtr = nullptr;
        }
    }
};

}

