#include "src/Type.h"

namespace h7l {

using U32 = unsigned int;
class GlobalContext;

struct Field{
    Type* type {nullptr};
    String name;
    U32 offset;
};

struct Class: public Type{
    List<Field> fields;
    U32 structSize {0};
    U32 arrayDimCnt {0};

    Class(){}
    Class(CString _id): id(_id){}

    void init(GlobalContext* gc, CList<Type*> types, CList<String> names);
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
