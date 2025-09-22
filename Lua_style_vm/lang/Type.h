#pragma once

#include <map>
#include "common/common.h"
#include "cons.h"

namespace h7l {

struct Value;
struct Type;
struct RuntimeContext;
class Scope;

struct Type{
    String id;
    int priType {kPriType_NONE};

    Type(){}
    Type(int priType):priType(priType){}
    Type(CString _id):id(_id){}
    virtual ~Type(){}

     bool isPrimetiveType()const{return priType != kPriType_NONE;}
};

}
