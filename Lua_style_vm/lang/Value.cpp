#include <unordered_map>
#include <cstring>
#include "Value.h"
#include "Object.h"

using namespace h7l;

Value::Value(Object* p, bool weakRef):objPtr(p), weakRef(weakRef){
    if(!weakRef){
        p->ref();
    }
}
Type* Value::getType(){
    return objPtr ? objPtr->getType() : nullptr;
}
void Value::deinit(){
    if(!weakRef && objPtr){
        objPtr->unref();
        objPtr = nullptr;
    }
}
void Value::toWeakRef(){
    if(!weakRef){
        if(objPtr->getRefCnt() > 1){
            objPtr->unref();
        }else{
            objPtr->unref();
            objPtr = nullptr;
        }
        weakRef = true;
    }
}
void Value::toStrongRef(){
    if(weakRef && objPtr){
        objPtr->ref();
        weakRef = false;
    }
}
bool Value::operator==(Value& v){
    return objPtr->compareEQ(*v.objPtr);
}
Value& Value::operator=(const Value& v){
    this->weakRef = v.weakRef;
    if(!weakRef){
        objPtr->ref();
    }
    this->objPtr = v.objPtr;
    return *this;
}
