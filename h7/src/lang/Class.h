#include "src/Type.h"

namespace h7l {

using U32 = unsigned int;
class GlobalContext;

enum ClassFlags{
    kClassFlag_ARRAY = 0x0001
};

struct Field{
    Type* type {nullptr};
    String name;
    U32 offset;
};

struct Class: public Type{
    List<Field> fields;
    U32 structSize {0};
    U32 flags {0};

    Class(){}
    Class(CString _id): id(_id){}

    void init(GlobalContext* gc, CList<Type*> types, CList<String> names);
    void initArray(){
        flags |= kClassFlag_ARRAY;
    }

    bool isArrayType()const{
        return (flags & kClassFlag_ARRAY) == kClassFlag_ARRAY;
    }
};

}
