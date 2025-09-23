#pragma once

#include <map>
#include "common/common.h"
#include "cons.h"

namespace h7l {

struct Value;
struct Type;
struct Class;
struct RuntimeContext;
class Scope;

struct Type{
    String id;
    int priType {kPriType_NONE};
    U32 arrayDimCnt {0};  //when is array-type. this > 0

    Type(){}
    Type(int priType):priType(priType){}
    Type(CString _id):id(_id){}
    virtual ~Type(){}

    //check base type is primetive or not
    bool isPrimetiveType()const{
        return priType != kPriType_NONE;
    }
    void initArray(int dimCnt){
        arrayDimCnt = dimCnt;
    }
    bool isArrayType()const{
        return arrayDimCnt > 0;
    }
    Class* asClass(){
        MED_ASSERT(!isPrimetiveType());
        return (Class*)this;
    }
    virtual void copyTo(Type* dst){
        dst->id = this->id;
        dst->priType = this->priType;
        dst->arrayDimCnt = this->arrayDimCnt;
    }
};

}
