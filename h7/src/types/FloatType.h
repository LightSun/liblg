#pragma once

#include "src/Type.h"
#include "src/Value.h"

namespace h7l {

struct FloatType : public Type{

    FloatType(){
        priType = kPriType_FLOAT;
        id = TYPE_ID_FLOAT;
        regFunc(kFuncOp_add, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_float += v2->as_float;
        });
        regFunc(kFuncOp_sub, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_float -= v2->as_float;
        });
        regFunc(kFuncOp_multi, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_float *= v2->as_float;
        });
        regFunc(kFuncOp_divide, [](RuntimeContext *, Value* v1, Value* v2){
//            if(v2->as_float != 0){
//                v1->as_float /= v2->as_float;
//            }else{
//                fprintf(stderr, "error divide-zero.\n");
//            }
        });
        regFunc(kFuncOp_copy, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_float = v2->as_float;
        });
        regFunc(kFuncOp_deepCopy, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_float = v2->as_float;
        });
    }
};

struct DoubleType : public Type{

    DoubleType(){
        priType = kPriType_DOUBLE;
        id = TYPE_ID_DOUBLE;
        regFunc(kFuncOp_add, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_double += v2->as_double;
        });
        regFunc(kFuncOp_sub, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_double -= v2->as_double;
        });
        regFunc(kFuncOp_multi, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_double *= v2->as_double;
        });
        regFunc(kFuncOp_divide, [](RuntimeContext *, Value* v1, Value* v2){
//            if(v2->as_double != 0){
//                v1->as_double /= v2->as_double;
//            }else{
//                fprintf(stderr, "error divide-zero.\n");
//            }
        });
        regFunc(kFuncOp_copy, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_double = v2->as_double;
        });
        regFunc(kFuncOp_deepCopy, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_double = v2->as_double;
        });
    }
};

}
