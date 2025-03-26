#include <memory.h>
#include "Object.h"
#include "src/lang/GlobalContext.h"

using namespace h7l;

void ShareData::unref(GlobalContext* gCtx){
    if(h_atomic_add(&_ref, -1) == 1){
        if(data){
            gCtx->getAllocator().free(data);
            data = nullptr;
        }
    }
}

Object::Object(GlobalContext* gCtx, Class* clsInfo):
    gCtx(gCtx),clsInfo(clsInfo){
    //clsInfo->structSize;
}

Object::~Object(){
    freeData();
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
    auto& alloc = gCtx->getAllocator();
    int actLen = buf.length() + 1;
    int mod = actLen % 8;
    int cap = mod > 0 ? (actLen / 8 + 1) * 8 : actLen;
    if(mb.data){
        mb.data = alloc.realloc(mb.data, cap);
    }else{
        mb.data = alloc.alloc(cap);
    }
    memcpy(mb.data, buf.data(), buf.length());
    mb.data[buf.length()] = '\0';
    mb.len = buf.length();
    mb.cap = cap;
}


