#pragma once

#include "src/Type.h"
#include "src/Value.h"

namespace h7l {

struct IntType : public Type{

    IntType(){
        priTYpe = kPriType_INT;
        id = TYPE_ID_INT;
        regFunc(kFuncOp_add, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_int += v2->as_int;
        });
        regFunc(kFuncOp_sub, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_int -= v2->as_int;
        });
        regFunc(kFuncOp_multi, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_int *= v2->as_int;
        });
        regFunc(kFuncOp_divide, [](RuntimeContext *, Value* v1, Value* v2){
//            if(v2->as_int != 0){
//                v1->as_int /= v2->as_int;
//            }else{
//                fprintf(stderr, "error divide-zero.\n");
//            }
        });
        regFunc(kFuncOp_mod, [](RuntimeContext *, Value* v1, Value* v2){
//            if(v2->as_int != 0){
//                v1->as_int %= v2->as_int;
//            }else{
//                fprintf(stderr, "error mode-zero.\n");
//            }
        });
        regFunc(kFuncOp_copy, [](RuntimeContext *, Value* v1, Value* v2){
           //v1->as_int = v2->as_int;
        });
        regFunc(kFuncOp_deepCopy, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_int = v2->as_int;
        });
    }
};

struct LongType : public Type{

    LongType(){
        priTYpe = kPriType_LONG;
        id = TYPE_ID_LONG;
        regFunc(kFuncOp_add, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_long += v2->as_long;
        });
        regFunc(kFuncOp_sub, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_long -= v2->as_long;
        });
        regFunc(kFuncOp_multi, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_long *= v2->as_long;
        });
        regFunc(kFuncOp_divide, [](RuntimeContext *, Value* v1, Value* v2){
//            if(v2->as_long != 0){
//                v1->as_long /= v2->as_long;
//            }else{
//                fprintf(stderr, "error divide-zero.\n");
//            }
        });
        regFunc(kFuncOp_mod, [](RuntimeContext *, Value* v1, Value* v2){
//            if(v2->as_long != 0){
//                v1->as_long %= v2->as_long;
//            }else{
//                fprintf(stderr, "error mode-zero.\n");
//            }
        });
        regFunc(kFuncOp_copy, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_long = v2->as_long;
        });
        regFunc(kFuncOp_deepCopy, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_long = v2->as_long;
        });
    }
};

struct CharType : public Type{

    CharType(){
        priTYpe = kPriType_CHAR;
        id = TYPE_ID_CHAR;
        regFunc(kFuncOp_add, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_char += v2->as_char;
        });
        regFunc(kFuncOp_sub, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_char -= v2->as_char;
        });
        regFunc(kFuncOp_multi, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_char *= v2->as_char;
        });
        regFunc(kFuncOp_divide, [](RuntimeContext *, Value* v1, Value* v2){
//            if(v2->as_char != 0){
//                v1->as_char /= v2->as_char;
//            }else{
//                fprintf(stderr, "error divide-zero.\n");
//            }
        });
        regFunc(kFuncOp_mod, [](RuntimeContext *, Value* v1, Value* v2){
//            if(v2->as_char != 0){
//                v1->as_char %= v2->as_char;
//            }else{
//                fprintf(stderr, "error mode-zero.\n");
//            }
        });
        regFunc(kFuncOp_copy, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_char = v2->as_char;
        });
        regFunc(kFuncOp_deepCopy, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_char = v2->as_char;
        });
    }
};

struct ShortType : public Type{

    ShortType(){
        priTYpe = kPriType_SHORT;
        id = TYPE_ID_SHORT;
        regFunc(kFuncOp_add, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_short += v2->as_short;
        });
        regFunc(kFuncOp_sub, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_short -= v2->as_short;
        });
        regFunc(kFuncOp_multi, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_short *= v2->as_short;
        });
        regFunc(kFuncOp_divide, [](RuntimeContext *, Value* v1, Value* v2){
//            if(v2->as_short != 0){
//                v1->as_short /= v2->as_short;
//            }else{
//                fprintf(stderr, "error divide-zero.\n");
//            }
        });
        regFunc(kFuncOp_mod, [](RuntimeContext *, Value* v1, Value* v2){
//            if(v2->as_short != 0){
//                v1->as_short %= v2->as_short;
//            }else{
//                fprintf(stderr, "error mode-zero.\n");
//            }
        });
        regFunc(kFuncOp_copy, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_short = v2->as_short;
        });
        regFunc(kFuncOp_deepCopy, [](RuntimeContext *, Value* v1, Value* v2){
            //v1->as_short = v2->as_short;
        });
    }
};
}
