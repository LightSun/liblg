#pragma once

#include "utils/h_atomic.h"
#include "common/common.h"

namespace h7l {

typedef unsigned int U32;

struct Class;
class GlobalContext;

enum MemoryFlags{
    kMemoryFlag_FREE    = 0x0001,
    kMemoryFlag_SHARE   = 0x0002,
    kMemoryFlag_NO_CARE = 0x0004,
};

enum ObjectFlags{
    kObjectFlag_MB_FREE,
    kObjectFlag_MB_SHARE,
    kObjectFlag_MB_NO_CARE,
};

struct ShareData{
   char* data {nullptr};
   volatile int _ref {1};

   void ref(){
       h_atomic_add(&_ref, 1);
   }
   //fetch_add
   void unref(GlobalContext* gCtx);
};

struct MemoryBlock{
    char* data {nullptr};
    U32 len;     //for string , exclude tail which is '\0'
    U32 cap {0}; //after align
};

struct Object{

    MemoryBlock mb;
    Class* clsInfo {nullptr};
    Object* super {nullptr};
    GlobalContext* gCtx {nullptr};
    volatile int _ref {1};
    U32 flags {0};
    U32 arrayLength {0};

    Object(GlobalContext* gCtx, Class* clsInfo);
    Object(){}
    ~Object();

    void ref(){
        h_atomic_add(&_ref, 1);
    }
    //fetch_add
    int unref(){
        return h_atomic_add(&_ref, -1);
    }
    bool hasFlag(int flag)const{ (flags & flag) == flag;}
    void freeData();
    void setStringAsData(CString buf);
};

}
