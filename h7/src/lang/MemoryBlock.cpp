#include <memory.h>
#include "MemoryBlock.h"
#include "src/lang/Scope.h"

using namespace h7l;

void MemoryBlock::freeData(){
    if(data){
        if(hasFlag(kMemoryFlag_FREE)){
            H7L_FREE(data);
        }else if(hasFlag(kMemoryFlag_SHARE)){
            ShareData* sd = (ShareData*)data;
            sd->unref();
        }
        data = nullptr;
    }
}
void MemoryBlock::setStringAsData(CString buf){
    if(hasFlag(kMemoryFlag_SHARE)){
        freeData();
    }
    int actLen = buf.length() + 1;
    int mod = actLen % 8;
    int cap = mod > 0 ? (actLen / 8 + 1) * 8 : actLen;
    if(data){
        data = H7L_REALLOC(data, cap);
    }else{
        data = H7L_ALLOC(cap);
    }
    memcpy(data, buf.data(), buf.length());
    dataPtr()[buf.length()] = '\0';
    this->len = buf.length();
    this->cap = cap;
    this->offset = 0;
    this->flags = kMemoryFlag_FREE;
}
