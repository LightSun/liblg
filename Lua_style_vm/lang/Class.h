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
    int getTotalElementCount(){
        if(shapes){
            int c = 1;
            for(auto& v : *shapes){
                c *= v;
            }
            return c;
        }
        return 0;
    }
    void get(Object* obj, Value* out);
    void set(Object* obj, Object* val);
    Object* getCopy(Object* obj);
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

    Class():Type(){
    }
    Class(CString _id): Type(_id){}

    String getName()const{return id;}
    Field& getFieldAt(int idx){return fields[idx];}
    int getFieldCount()const {return fields.size();}

    void init(GlobalContext* gc, CList<MemberInfo> types, CList<String> names);

    void copyTo(Type* _dst)override{
        Type::copyTo(_dst);
        MED_ASSERT(!_dst->isPrimetiveType());
        //
        Class* dst = (Class*)(_dst);
        dst->parent = this->parent;
        dst->fields = this->fields;
        dst->structSize = this->structSize;
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
