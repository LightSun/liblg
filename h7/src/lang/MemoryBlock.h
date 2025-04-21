#pragma once

#include <memory.h>
#include "utils/h_atomic.h"
#include "common/common.h"
#include "src/lang/Allocator.h"

namespace h7l {

enum MemoryFlags{
    kMemoryFlag_FREE    = 0x0001,
    kMemoryFlag_SHARE   = 0x0002,
    kMemoryFlag_NO_CARE = 0x0004,
};

struct ShareData{
    void* data {nullptr};
    volatile int _ref {1};

    static ShareData* New(size_t alignSize){
        //auto sd = (ShareData*)H7L_ALLOC(sizeof(ShareData));
        auto sd = new ShareData();
        sd->data = malloc(alignSize);
        sd->_ref = 1;
        return sd;
    }

    void ref(){
        h_atomic_add(&_ref, 1);
    }
    //fetch_add
    void unref(){
        if(h_atomic_add(&_ref, -1) == 1){
            if(data){
                free(data);
                data = nullptr;
            }
            delete (this);
        }
    }
};

struct MemoryBlock{
    void* data {nullptr};
    U32 len;        //for string , exclude tail which is '\0'
    U32 cap {0};    //after align
    U32 offset {0}; //offset of share-data
    U32 flags {0};

    ~MemoryBlock(){freeData();}

    char* dataPtr(){
        return (char*)data;
    }
    String asString(){
        return String(dataPtr(), len);
    }
    bool hasFlag(U32 flag)const{ return (flags & flag) == flag;}
    void addFlag(U32 flag){ flags |= flag;}

    void setStringAsData(CString buf);
    void freeData();

    void initWithShareData(ShareData* sd, int _len, int _alignSize){
        data = sd;
        len = _len;
        cap = _alignSize;
        offset = 0;
        addFlag(kMemoryFlag_SHARE);
    }
    void initWithStructSize(size_t structSize){
        data = malloc(structSize);
        len = structSize;
        cap = structSize;
        offset = 0;
        addFlag(kMemoryFlag_FREE);
    }
    void reset(){
        memset(this, 0, sizeof(MemoryBlock));
    }
};
}
