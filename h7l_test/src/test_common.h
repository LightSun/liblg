#pragma once

#include "common/common.h"
#include "h7/src/lang/Object.h"
#include "h7/src/lang/Class.h"
#include "h7/src/lang/Scope.h"

namespace h7l {

struct ObjectDeleter{
    void operator()(Object* obj)const{
        obj->unref();
    }
};

struct TestContext{
    std::unique_ptr<GlobalContext> gc;
    std::unique_ptr<Scope> scope;
    std::unique_ptr<Object, ObjectDeleter> obj;

    TestContext(){
        gc = std::make_unique<GlobalContext>();
        scope = std::make_unique<Scope>(gc.get());

        obj.reset(new Object());
        obj->scope = scope.get();
    }
    ~TestContext(){
        //fprintf(stderr, "~TestContext\n");
    }
    Object* getObjPtr(){return obj.get();}
    Object& getObj(){return *obj;}
};
}
