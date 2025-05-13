#pragma once

#include <map>
#include "common/common.h"
#include "src/cons.h"

namespace h7l {

struct Value;
struct Type;
struct RuntimeContext;

struct Type{
    String id;
    int priType {kPriType_NONE};

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
    bool isPrimetiveType()const{return priType != kPriType_NONE;}

public:
    //dst, src
    std::map<int, std::function<void(RuntimeContext*, Value*, Value*)>> funcMap;
    std::map<std::string, std::function<void(RuntimeContext*, Value*, CList<Value*>)>> selfFuncMap;
};
}
