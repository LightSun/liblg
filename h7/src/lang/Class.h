#pragma once

#include "src/Type.h"

namespace h7l {

using U32 = unsigned int;
class GlobalContext;

struct Value;
struct Object;

struct Field{
    Type* type {nullptr};
    String name;
    std::unique_ptr<List<int>> shapes;
    U32 offset {0};

    Field(){}
    Field(const Field& f){
        this->type = f.type;
        this->name = f.name;
        this->offset = f.offset;
        if(!f.shapes->empty()){
            this->shapes = std::make_unique<List<int>>();
            *this->shapes = *f.shapes;
        }else{
            this->shapes = nullptr;
        }
    }
    Field& operator=(const Field& f){
        this->type = f.type;
        this->name = f.name;
        this->offset = f.offset;
        if(!f.shapes->empty()){
            this->shapes = std::make_unique<List<int>>();
            *this->shapes = *f.shapes;
        }else{
            this->shapes = nullptr;
        }
        return *this;
    }
    void setShapes(CList<int> s){
        if(!s.empty()){
            shapes = std::make_unique<List<int>>();
            *shapes = s;
        }else{
            shapes = nullptr;
        }
    }
    void get(Object* obj, Value* out);
};

struct MemberInfo{
    Type* type {nullptr};
    List<int> shapes;

    MemberInfo(){}
    MemberInfo(Type* type, CList<int> shapes = {}):
        type(type), shapes(shapes){}
};

struct Class: public Type{
    List<Field> fields;
    U32 structSize {0};
    U32 arrayDimCnt {0};

    Class():Type(){
    }
    Class(CString _id): Type(_id){}

    String getName()const{return id;}
    Field& getFieldAt(int idx){return fields[idx];}

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
    void copyTo(Class* dst){
        Type::copyTo(dst);
        dst->fields = this->fields;
        dst->structSize = this->structSize;
        dst->arrayDimCnt = this->arrayDimCnt;
    }
    void toArrayType(int dim, Class* outC){
        copyTo(outC);
        outC->arrayDimCnt = dim;
    }
};

}
