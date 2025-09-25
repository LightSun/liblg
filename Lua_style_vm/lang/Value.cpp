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
Value::~Value(){
    deinit();
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
Value::Value(Value&& val){
    this->objPtr = val.objPtr;
    this->weakRef = val.weakRef;
    if(!weakRef){
        objPtr->ref();
    }
}
Value::Value(const Value& val){
    this->objPtr = val.objPtr;
    this->weakRef = val.weakRef;
    if(!weakRef){
        objPtr->ref();
    }
}
bool Value::operator==(Value& v){
    return objPtr->compareEQ(*v.objPtr);
}
Value& Value::operator=(const Value& v){
    this->weakRef = v.weakRef;
    this->objPtr = v.objPtr;
    if(!weakRef){
        objPtr->ref();
    }
    return *this;
}
Value& Value::operator=(Value&& v){
    this->weakRef = v.weakRef;
    this->objPtr = v.objPtr;
    if(!weakRef){
        objPtr->ref();
    }
    return *this;
}
