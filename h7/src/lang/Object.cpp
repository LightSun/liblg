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
    unref();
}
void Object::unref(){
    if(h_atomic_add(&_ref, -1) == 1){
        mb.freeData();
        delete this;
    }
}
U32 Object::dataSize(){
    if(arrayDesc){
        return arrayDesc->eleCount * clsInfo->structSize;
    }
    return clsInfo->structSize;
}
Object* Object::subArray(int index){
    Object* obj = nullptr;
    if(isArray()){
        U32 offset = 0;
        auto newDesc = std::make_unique<ArrayDesc>(clsInfo->structSize);
        if(arrayDesc->toSubArray(index, offset, *newDesc)){
            //
            U32 actDataSize = newDesc->eleCount * clsInfo->structSize;
            U32 alignSize;
            if(clsInfo->isPrimiveType()){
                alignSize = actDataSize;
            }else{
                alignSize = 8 - actDataSize % 8 + actDataSize;
            }
            //
            obj = new Object();
            obj->mb.reset();
            ShareData* sd = (ShareData*)mb.data;
            sd->ref();
            size_t newOffset = mb.offset + offset;
            //
            obj->init0(scope, clsInfo, sd, std::move(newDesc));
            obj->mb.offset = newOffset;
            return obj;
        }else{
            fprintf(stderr, "index out of range.\n");
        }
    }
    return nullptr;
}
//-----------------------
void Object::init0(Scope* scope, Class* clsInfo, ShareData* sd, std::unique_ptr<ArrayDesc> desc){
    MED_ASSERT_X(!clsInfo->isArrayType(), "clsInfo must not be array type.");
    this->scope = scope;
    this->clsInfo = clsInfo;
    this->super = nullptr;
    this->_ref = 1;
    this->flags = 0;
    if(desc){
        arrayDesc = std::move(desc);
        U32 actDataSize = arrayDesc->eleCount * clsInfo->structSize;
        U32 alignSize;
        if(clsInfo->isPrimiveType()){
            alignSize = actDataSize;
        }else{
            alignSize = 8 - actDataSize % 8 + actDataSize;
        }
        if(sd == nullptr){
            sd = ShareData::New(alignSize);
        }
        if(mb.data == nullptr){
            mb.initWithShareData(sd, actDataSize, alignSize);
        }
    }else{
        this->arrayDesc = nullptr;
        mb.initWithStructSize(clsInfo->structSize);
    }
}


