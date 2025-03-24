#pragma once

#include "src/Type.h"
#include "src/Value.h"

namespace h7l {

struct IntType : public Type{

    IntType(){
        id = "int";
        regFunc(kFuncOp_add, [](TypeContext *, Value* v1, Value* v2){
            v1->as_int += v2->as_int;
        });
        regFunc(kFuncOp_sub, [](TypeContext *, Value* v1, Value* v2){
            v1->as_int -= v2->as_int;
        });
        regFunc(kFuncOp_multi, [](TypeContext *, Value* v1, Value* v2){
            v1->as_int *= v2->as_int;
        });
        regFunc(kFuncOp_divide, [](TypeContext *, Value* v1, Value* v2){
            if(v2->as_int != 0){
                v1->as_int /= v2->as_int;
            }else{
                fprintf(stderr, "error divide-zero.\n");
            }
        });
        regFunc(kFuncOp_mod, [](TypeContext *, Value* v1, Value* v2){
            if(v2->as_int != 0){
                v1->as_int %= v2->as_int;
            }else{
                fprintf(stderr, "error mode-zero.\n");
            }
        });
        regFunc(kFuncOp_copy, [](TypeContext *, Value* v1, Value* v2){
           v1->as_int %= v2->as_int;
        });
    }
};
}
