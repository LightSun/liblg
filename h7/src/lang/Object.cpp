#include <memory.h>
#include "Object.h"
#include "src/lang/Scope.h"

using namespace h7l;

Object::Object(Scope* scope, Class* clsInfo, CList<int> shapes){
    MED_ASSERT_X(!clsInfo->isArrayType(), "clsInfo must not be array type.");
    std::unique_ptr<ArrayDesc> desc;
    if(!shapes.empty()){
        desc = std::make_unique<ArrayDesc>(clsInfo->structSize);
        desc->setShape(shapes);
    }
    init0(scope, clsInfo, nullptr, std::move(desc));
}
Object::Object(Scope* scope, Class* clsInfo, std::unique_ptr<ArrayDesc> desc){
    MED_ASSERT_X(!clsInfo->isArrayType(), "clsInfo must not be array type.");
    init0(scope, clsInfo, nullptr, std::move(desc));
}

Object::~Object(){
    //fprintf(stderr,"Object::~Object\n");
    unref();
}
void Object::unref(){
    if(h_atomic_add(&_ref, -1) == 1){
        mb.freeData();
        delete this;
    }
}
U32 Object::getDataSize(){
    auto clsInfo = getClass();
    if(clsInfo){
        if(arrayDesc){
            return arrayDesc->eleCount * clsInfo->structSize;
        }
        return clsInfo->structSize;
    }else{
        return mb.getPrimitiveSize();
    }
}
U32 Object::getBaseDataSize(){
    auto clsInfo = getClass();
    if(clsInfo){
        return clsInfo->structSize;
    }
    return mb.getPrimitiveSize();
}
void Object::reset(){
    mb.freeData();
    type = nullptr;
    super = nullptr;
    arrayDesc = nullptr;
    flags = 0;
}
Object* Object::subArray(int index){
    Object* obj = nullptr;
    if(isArray()){
        U32 offset = 0;
        auto newDesc = std::make_unique<ArrayDesc>(getBaseDataSize());
        if(arrayDesc->toSubArray(index, offset, *newDesc)){
            //
            // U32 actDataSize = newDesc->eleCount * clsInfo->structSize;
            // U32 alignSize;
            // if(clsInfo->isPrimiveType()){
            //     alignSize = actDataSize;
            // }else{
            //     alignSize = 8 - actDataSize % 8 + actDataSize;
            // }
            //
            obj = new Object();
            obj->mb.reset();
            ShareData* sd = (ShareData*)mb.data;
            sd->ref();
            size_t newOffset = mb.offset + offset;
            //
            obj->init0(scope, this->type, sd, std::move(newDesc));
            obj->mb.offset = newOffset;
            return obj;
        }else{
            fprintf(stderr, "index out of range.\n");
        }
    }
    return nullptr;
}
bool Object::castPrimitive(int priType){
    MED_ASSERT(scope);
    auto oldType = mb.getPrimitiveType();
    if(oldType == priType){
        return true;
    }
    char buf[sizeof(void*)];
    int oldSize = 0;
    if(!mb.getPrimitiveValue(buf, &oldSize)){
        return false;
    }
    auto gc = scope->getGlobalContext();
    this->type = gc->getPrimitiveType(priType);
    auto newSize = primitive_get_size(priType);
    //
    if(oldSize == newSize){
        if(primitive_isIntLike(oldType) && primitive_isIntLike(priType)){
            mb.initWithWrapPrimitiveWithRawAddr(priType, buf);
        }else{
            mb.initWithWrapPrimitiveWithRawAddr2(priType, buf);
        }
        return true;
    }
    mb.freeData();

    mb.initWithWrapPrimitive2(priType, buf);
    return true;
}
bool Object::castPrimitiveTo(int priType, void* newPtr){
    MED_ASSERT(scope);
    auto oldType = mb.getPrimitiveType();
    if(oldType == priType){
        return true;
    }
    char buf[sizeof(void*)];
    int oldSize = 0;
    if(!mb.getPrimitiveValue(buf, &oldSize)){
        return false;
    }
    auto gc = scope->getGlobalContext();
    this->type = gc->getPrimitiveType(priType);
    //
    mb.freeData();
    mb.initWithWrapPrimitivePtr(priType, buf, newPtr);
    return true;
}
//-----------------------
void Object::init0(Scope* scope, Type* _type, ShareData* sd, std::unique_ptr<ArrayDesc> desc){
    MED_ASSERT_X(_type, "type must be valid.");
    Class* cls = nullptr;
    if(!_type->isPrimetiveType()){
        cls = (Class*)_type;
        MED_ASSERT_X(!cls->isArrayType(), "clsInfo must not be array type.");
    }
    this->scope = scope;
    this->type = _type;
    this->super = nullptr;
    this->_ref = 1;
    this->flags = 0;
    if(desc){
        arrayDesc = std::move(desc);
        const int unitSize = cls ? cls->structSize : primitive_get_size(_type->priType);
        U32 actDataSize;
        U32 alignSize;
        actDataSize = arrayDesc->eleCount * unitSize;
        if(_type->isPrimetiveType()){
            alignSize = actDataSize;
        }else{
            alignSize = sizeof(void*) - actDataSize % sizeof(void*) + actDataSize;
        }
        if(sd == nullptr){
            sd = ShareData::New(alignSize);
        }
        if(mb.data == nullptr){
            mb.initWithShareData(sd, actDataSize, alignSize);
        }
    }else{
        this->arrayDesc = nullptr;
        if(cls){
            mb.initWithStructSize(cls->structSize);
        }
    }
}


