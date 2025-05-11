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
    bool temp {true};
    IObject* objPtr {nullptr};

    Type* getType(){
//TODO
    }
    void deinit(){
        if(objPtr && !temp){
            objPtr->unref();
            objPtr = nullptr;
        }
    }
};

}

