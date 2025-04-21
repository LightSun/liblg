#pragma once

#include <map>
#include "common/common.h"

#define TYPE_ID_INT "int"
#define TYPE_ID_CHAR "char"
#define TYPE_ID_SHORT "short"
#define TYPE_ID_LONG "long"
#define TYPE_ID_FLOAT "float"
#define TYPE_ID_DOUBLE "double"

namespace h7l {

struct Value;
struct Type;
struct RuntimeContext;

enum FuncOp{
    kFuncOp_add,
    kFuncOp_sub,
    kFuncOp_multi,
    kFuncOp_divide,
    kFuncOp_mod,
    kFuncOp_copy,
    kFuncOp_deepCopy,
};

struct Type{
    String id;

    Type(){}
    Type(CString _id):id(_id){}
    virtual ~Type(){}

    void copyTo(Type* dst){
        dst->id = this->id;
        dst->funcMap = dst->funcMap;
        dst->selfFuncMap = dst->selfFuncMap;
    }
    void regFunc(int op, std::function<void(RuntimeContext *, Value*, Value*)> func){
        funcMap[op] = func;
    }
    void regSelfFunc(CString op, std::function<void(RuntimeContext *, Value*,
                                                    CList<Value*>)> func){
        selfFuncMap[op] = func;
    }
    bool call(enum FuncOp op, RuntimeContext* rc, Value* v1, Value* v2){
        auto it = funcMap.find(op);
        if(it != funcMap.end()){
            it->second(rc, v1, v2);
            return true;
        }
        return false;
    }
    bool callSelf(CString op, RuntimeContext* rc, Value* ret, CList<Value*> params){
        auto it = selfFuncMap.find(op);
        if(it != selfFuncMap.end()){
            it->second(rc, ret, params);
            return true;
        }
        return false;
    }
    virtual bool isPrimetiveType(){return true;}

public:
    //dst, src
    std::map<int, std::function<void(RuntimeContext*, Value*, Value*)>> funcMap;
    std::map<std::string, std::function<void(RuntimeContext*, Value*, CList<Value*>)>> selfFuncMap;
};
}
