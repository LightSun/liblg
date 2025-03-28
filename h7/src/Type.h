#pragma once

#include <map>
#include "common/common.h"

namespace h7l {

struct Value;
struct VM;
struct Type;

struct TypeContext{
    Type* type {nullptr};
    VM* vm {nullptr};
};

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

    void regFunc(int op, std::function<void(TypeContext *, Value*, Value*)> func){
        funcMap[op] = func;
    }
    void regSelfFunc(CString op, std::function<void(TypeContext *, Value*,
                                                    CList<Value*>)> func){
        selfFuncMap[op] = func;
    }
    bool call(enum FuncOp op, VM *vm, Value* v1, Value* v2){
        auto it = funcMap.find(op);
        if(it != funcMap.end()){
            TypeContext tc;
            tc.type = this;
            tc.vm = vm;
            it->second(&tc, v1, v2);
            return true;
        }
        return false;
    }
    bool callSelf(CString op, VM *vm, Value* ret, CList<Value*> params){
        auto it = selfFuncMap.find(op);
        if(it != selfFuncMap.end()){
            TypeContext tc;
            tc.type = this;
            tc.vm = vm;
            it->second(&tc, ret, params);
            return true;
        }
        return false;
    }
    virtual bool isPrimetiveType(){return true;}

private:
    //dst, src
    std::map<int, std::function<void(TypeContext*, Value*, Value*)>> funcMap;
    std::map<std::string, std::function<void(TypeContext*, Value*, CList<Value*>)>> selfFuncMap;
};
}
