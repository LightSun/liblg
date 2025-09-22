#pragma once

#include "Type.h"

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
    Class* parent {nullptr};
    List<Field> fields;
    U32 structSize {0};
    U32 arrayDimCnt {0};

    Class():Type(){
    }
    Class(CString _id): Type(_id){}

    String getName()const{return id;}
    Field& getFieldAt(int idx){return fields[idx];}
    int getFieldCount()const {return fields.size();}

    void init(GlobalContext* gc, CList<MemberInfo> types, CList<String> names);

    void initArray(int dimCnt){
        arrayDimCnt = dimCnt;
    }
    bool isArrayType()const{
        return arrayDimCnt > 0;
    }
    void copyTo(Class* dst){
        dst->id = this->id;
        dst->priType = this->priType;
        dst->fields = this->fields;
        dst->structSize = this->structSize;
        dst->arrayDimCnt = this->arrayDimCnt;
    }
    void toArrayType(int dim, Class* outC){
        copyTo(outC);
        outC->arrayDimCnt = dim;
    }
public:
    std::vector<Class*> getClasses(){
        std::vector<Class*> vec;
        vec.push_back(this);
        getSuperClasses0(vec);
        return vec;
    }
    std::vector<Class*> getSuperClasses(){
        std::vector<Class*> vec;
        getSuperClasses0(vec);
        return vec;
    }
    bool hasSuperClass(Class* super){
        std::vector<Class*> vec;
        getSuperClasses0(vec);
        for(auto& s : vec){
            if(s == super){
                return true;
            }
        }
        return false;
    }
private:
    void getSuperClasses0(std::vector<Class*>& vec){
        Class* p = parent;
        while ( p != nullptr) {
            vec.push_back(p);
            p = p->parent;
        }
    }
};

}
