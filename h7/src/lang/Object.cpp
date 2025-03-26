#include <memory.h>
#include "Object.h"
#include "src/lang/Scope.h"

using namespace h7l;

void ShareData::unref(Allocator* aoc){
    if(h_atomic_add(&_ref, -1) == 1){
        if(data){
            aoc->free(data);
            data = nullptr;
        }
    }
}

Object::Object(Scope* scope, Class* clsInfo, CList<int> shapes):
    scope(scope),clsInfo(clsInfo){
    MED_ASSERT_X(!clsInfo->isArrayType(), "clsInfo must not be array type.");
    if(!shapes.empty()){
        arrayDesc = std::make_unique<ArrayDesc>();
        arrayDesc->setShape(clsInfo->structSize, shapes);
        U32 actDataSize = arrayDesc->eleCount * clsInfo->structSize;
        U32 alignSize;
        if(clsInfo->isPrimiveType()){
            alignSize = actDataSize;
        }else{
            alignSize = 8 - actDataSize % 8 + actDataSize;
        }
        ShareData* sd = scope->getAllocator()->alloc(sizeof(ShareData));
        sd->data = scope->getAllocator()->alloc(alignSize);
        sd->offset = 0;
        sd->_ref = 1;
        mb.data = sd;
        mb.len = actDataSize;
        mb.cap = alignSize;
        addFlag(kObjectFlag_MB_SHARE);
    }else{
        size_t structSize = clsInfo->structSize;
        mb.data = scope->getAllocator()->alloc(structSize);
        mb.len = structSize;
        mb.cap = structSize;
        addFlag(kObjectFlag_MB_FREE);
    }
}

Object::~Object(){
    unref();
}
void Object::unref(){
    if(h_atomic_add(&_ref, -1) == 1){
        freeData();
        scope->getAllocator()->free(this);
    }
}
U32 Object::dataSize(){
    if(arrayDesc){
        return arrayDesc->eleCount * clsInfo->structSize;
    }
    return clsInfo->structSize;
}
void Object::freeData(){
    if(mb.data){
        if(hasFlag(kObjectFlag_MB_FREE)){
            gCtx->getAllocator().free(mb.data);
        }else if(hasFlag(kObjectFlag_MB_SHARE)){
            ShareData* sd = (ShareData*)mb.data;
            sd->unref(gCtx);
        }
        mb.data = nullptr;
    }
}
void Object::setStringAsData(CString buf){
    if(hasFlag(kObjectFlag_MB_SHARE)){
        freeData();
    }
    auto& alloc = scope->getAllocator();
    int actLen = buf.length() + 1;
    int mod = actLen % 8;
    int cap = mod > 0 ? (actLen / 8 + 1) * 8 : actLen;
    if(mb.data){
        mb.data = alloc->realloc(mb.data, cap);
    }else{
        mb.data = alloc->alloc(cap);
    }
    memcpy(mb.data, buf.data(), buf.length());
    mb.data[buf.length()] = '\0';
    mb.len = buf.length();
    mb.cap = cap;
}


