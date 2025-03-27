#pragma once

#include "src/Type.h"
#include "src/Value.h"
#include "src/lang/Object.h"
#include "src/lang/GlobalContext.h"
#include "src/VM.h"

namespace h7l {

//struct StringType : public Type{

//    StringType(){
//        id = "h7l.lang.String";
//        regFunc(kFuncOp_add, [](TypeContext * tc, Value* v1, Value* v2){
//            Object* obj1 = (Object*)v1->as_ptr;
//            Object* obj2 = (Object*)v2->as_ptr;
//            String str;
//            str.append(String(obj1->mb.dataPtr(), obj1->data.len));
//            str.append(String(obj2->data.data, obj2->data.len));
//            obj1->setStringAsData(str);
//        });
//        regFunc(kFuncOp_copy, [](TypeContext *, Value* v1, Value* v2){
//            Object* obj1 = (Object*)v1->as_ptr;
//            Object* obj2 = (Object*)v2->as_ptr;
//            obj1->setStringAsData(String(obj2->data.data, obj2->data.len));
//        });
//        regFunc(kFuncOp_deepCopy, [](TypeContext *, Value* v1, Value* v2){
//            Object* obj1 = (Object*)v1->as_ptr;
//            Object* obj2 = (Object*)v2->as_ptr;
//            obj1->setStringAsData(String(obj2->data.data, obj2->data.len));
//        });
//    }
//};
}
