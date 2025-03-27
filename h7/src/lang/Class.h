#pragma once
#include "src/Type.h"

namespace h7l {

using U32 = unsigned int;
class GlobalContext;

struct Field{
    Type* type {nullptr};
    String name;
    std::unique_ptr<List<int>> shapes;
    U32 offset;

    void setShapes(CList<int> s){
        if(!s.empty()){
            shapes = std::make_unique<List<int>>();
            *shapes = s;
        }else{
            shapes = nullptr;
        }
    }
};

struct MemberInfo{
    Type* type {nullptr};
    List<int> shapes;
};

struct Class: public Type{
    List<Field> fields;
    U32 structSize {0};
    U32 arrayDimCnt {0};

    Class():Type(){
    }
    Class(CString _id): Type(_id){}

    void init(GlobalContext* gc, CList<MemberInfo> types, CList<String> names);

    void initArray(int dimCnt){
        arrayDimCnt = dimCnt;
    }
    bool isArrayType()const{
        return arrayDimCnt > 0;
    }
    bool isPrimiveType()const{
        return id.find(".") == String::npos;
    }
};

}
