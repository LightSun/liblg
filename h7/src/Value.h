#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "src/Type.h"

namespace h7l {

struct Object;

struct Value {
    Object* objPtr {nullptr};
    bool weakRef {true};

    Value(){}
    Value(Object* p, bool weakRef);

    Type* getType();
    void deinit();

    bool isStrongRef()const{return !weakRef;}
    bool isWeakRef()const{return weakRef;}
    void toWeakRef();
    void toStrongRef();

    bool operator==(Value& v);

    Value& operator=(const Value& v);
};

struct ValueHolder{
    std::unique_ptr<Value> ptr;

    ValueHolder(Object* p, bool weakRef){
        ptr = std::make_unique<Value>(p, weakRef);
    }
    ~ValueHolder(){
        ptr->deinit();
    }
    Value* get(){
        return ptr.get();
    }
};

}

