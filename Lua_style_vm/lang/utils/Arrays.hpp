#pragma once

#include <stdlib.h>
#include "utils/Numbers.hpp"

namespace h7l {

struct ArrayHolder{
    void* ptr {nullptr};
    size_t size {0};
    size_t alignSize {0};

    ArrayHolder(size_t s):size(s){
        alignSize = roundUp8(s);
        ptr = malloc(alignSize);
    }
    ~ArrayHolder(){
        if(ptr){
            free(ptr);
            ptr = nullptr;
        }
    }
};

}
