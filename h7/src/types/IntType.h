#pragma once

#include "src/Type.h"
#include "src/Value.h"

namespace h7l {

struct IntType : public Type{

    IntType(){
        size = sizeof(int);
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
           v1->as_int = v2->as_int;
        });
        regFunc(kFuncOp_deepCopy, [](TypeContext *, Value* v1, Value* v2){
            v1->as_int = v2->as_int;
        });
    }
};

struct LongType : public Type{

    LongType(){
        size = sizeof(int64_t);
        id = "long";
        regFunc(kFuncOp_add, [](TypeContext *, Value* v1, Value* v2){
            v1->as_long += v2->as_long;
        });
        regFunc(kFuncOp_sub, [](TypeContext *, Value* v1, Value* v2){
            v1->as_long -= v2->as_long;
        });
        regFunc(kFuncOp_multi, [](TypeContext *, Value* v1, Value* v2){
            v1->as_long *= v2->as_long;
        });
        regFunc(kFuncOp_divide, [](TypeContext *, Value* v1, Value* v2){
            if(v2->as_long != 0){
                v1->as_long /= v2->as_long;
            }else{
                fprintf(stderr, "error divide-zero.\n");
            }
        });
        regFunc(kFuncOp_mod, [](TypeContext *, Value* v1, Value* v2){
            if(v2->as_long != 0){
                v1->as_long %= v2->as_long;
            }else{
                fprintf(stderr, "error mode-zero.\n");
            }
        });
        regFunc(kFuncOp_copy, [](TypeContext *, Value* v1, Value* v2){
            v1->as_long = v2->as_long;
        });
        regFunc(kFuncOp_deepCopy, [](TypeContext *, Value* v1, Value* v2){
            v1->as_long = v2->as_long;
        });
    }
};

struct CharType : public Type{

    CharType(){
        size = sizeof(char);
        id = "char";
        regFunc(kFuncOp_add, [](TypeContext *, Value* v1, Value* v2){
            v1->as_char += v2->as_char;
        });
        regFunc(kFuncOp_sub, [](TypeContext *, Value* v1, Value* v2){
            v1->as_char -= v2->as_char;
        });
        regFunc(kFuncOp_multi, [](TypeContext *, Value* v1, Value* v2){
            v1->as_char *= v2->as_char;
        });
        regFunc(kFuncOp_divide, [](TypeContext *, Value* v1, Value* v2){
            if(v2->as_char != 0){
                v1->as_char /= v2->as_char;
            }else{
                fprintf(stderr, "error divide-zero.\n");
            }
        });
        regFunc(kFuncOp_mod, [](TypeContext *, Value* v1, Value* v2){
            if(v2->as_char != 0){
                v1->as_char %= v2->as_char;
            }else{
                fprintf(stderr, "error mode-zero.\n");
            }
        });
        regFunc(kFuncOp_copy, [](TypeContext *, Value* v1, Value* v2){
            v1->as_char = v2->as_char;
        });
        regFunc(kFuncOp_deepCopy, [](TypeContext *, Value* v1, Value* v2){
            v1->as_char = v2->as_char;
        });
    }
};

struct ShortType : public Type{

    ShortType(){
        size = sizeof(short);
        id = "short";
        regFunc(kFuncOp_add, [](TypeContext *, Value* v1, Value* v2){
            v1->as_short += v2->as_short;
        });
        regFunc(kFuncOp_sub, [](TypeContext *, Value* v1, Value* v2){
            v1->as_short -= v2->as_short;
        });
        regFunc(kFuncOp_multi, [](TypeContext *, Value* v1, Value* v2){
            v1->as_short *= v2->as_short;
        });
        regFunc(kFuncOp_divide, [](TypeContext *, Value* v1, Value* v2){
            if(v2->as_short != 0){
                v1->as_short /= v2->as_short;
            }else{
                fprintf(stderr, "error divide-zero.\n");
            }
        });
        regFunc(kFuncOp_mod, [](TypeContext *, Value* v1, Value* v2){
            if(v2->as_short != 0){
                v1->as_short %= v2->as_short;
            }else{
                fprintf(stderr, "error mode-zero.\n");
            }
        });
        regFunc(kFuncOp_copy, [](TypeContext *, Value* v1, Value* v2){
            v1->as_short = v2->as_short;
        });
        regFunc(kFuncOp_deepCopy, [](TypeContext *, Value* v1, Value* v2){
            v1->as_short = v2->as_short;
        });
    }
};
}
