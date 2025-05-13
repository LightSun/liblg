#include <unordered_map>
#include <cstring>
#include "src/Value.h"
#include "src/lang/Object.h"

using namespace h7l;

Value::Value(Object* p, bool weakRef):objPtr(p), weakRef(weakRef){
    if(!weakRef){
        p->ref();
    }
}
Type* Value::getType(){
    return objPtr ? (Type*)objPtr->clsInfo : nullptr;
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
Value& Value::operator=(const Value& v){
    this->weakRef = v.weakRef;
    if(!weakRef){
        objPtr->ref();
    }
    this->objPtr = v.objPtr;
    return *this;
}
