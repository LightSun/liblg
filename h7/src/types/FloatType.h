#pragma once

#include "src/Type.h"
#include "src/Value.h"

namespace h7l {

struct FloatType : public Type{

    FloatType(){
        size = sizeof(float);
        id = "float";
        regFunc(kFuncOp_add, [](TypeContext *, Value* v1, Value* v2){
            v1->as_float += v2->as_float;
        });
        regFunc(kFuncOp_sub, [](TypeContext *, Value* v1, Value* v2){
            v1->as_float -= v2->as_float;
        });
        regFunc(kFuncOp_multi, [](TypeContext *, Value* v1, Value* v2){
            v1->as_float *= v2->as_float;
        });
        regFunc(kFuncOp_divide, [](TypeContext *, Value* v1, Value* v2){
            if(v2->as_float != 0){
                v1->as_float /= v2->as_float;
            }else{
                fprintf(stderr, "error divide-zero.\n");
            }
        });
        regFunc(kFuncOp_copy, [](TypeContext *, Value* v1, Value* v2){
           v1->as_float = v2->as_float;
        });
        regFunc(kFuncOp_deepCopy, [](TypeContext *, Value* v1, Value* v2){
            v1->as_float = v2->as_float;
        });
    }
};

struct DoubleType : public Type{

    DoubleType(){
        size = sizeof(double);
        id = "double";
        regFunc(kFuncOp_add, [](TypeContext *, Value* v1, Value* v2){
            v1->as_double += v2->as_double;
        });
        regFunc(kFuncOp_sub, [](TypeContext *, Value* v1, Value* v2){
            v1->as_double -= v2->as_double;
        });
        regFunc(kFuncOp_multi, [](TypeContext *, Value* v1, Value* v2){
            v1->as_double *= v2->as_double;
        });
        regFunc(kFuncOp_divide, [](TypeContext *, Value* v1, Value* v2){
            if(v2->as_double != 0){
                v1->as_double /= v2->as_double;
            }else{
                fprintf(stderr, "error divide-zero.\n");
            }
        });
        regFunc(kFuncOp_copy, [](TypeContext *, Value* v1, Value* v2){
            v1->as_double = v2->as_double;
        });
        regFunc(kFuncOp_deepCopy, [](TypeContext *, Value* v1, Value* v2){
            v1->as_double = v2->as_double;
        });
    }
};

}
