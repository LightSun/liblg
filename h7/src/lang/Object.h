#pragma once

#include "utils/h_atomic.h"
#include "common/common.h"

namespace h7l {

typedef unsigned int U32;

struct Class;
struct Allocator;
class Scope;

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
   U32 offset {0};  // can be used for array-nested

   void ref(){
       h_atomic_add(&_ref, 1);
   }
   //fetch_add
   void unref(Allocator* aoc);
};

struct MemoryBlock{
    char* data {nullptr};
    U32 len;     //for string , exclude tail which is '\0'
    U32 cap {0}; //after align
};

struct ArrayDesc{
    List<int> shapes;
    List<int> strides;
    U32 eleCount {0};

    void setShape(U32 baseSize,CList<int> shapes){
        this->shapes = shapes;
        strides.resize(shapes.size());
        const int size = shapes.size();
        U32 eleSize = baseSize;
        int eleC = 1;
        for(int i = size - 1 ; i >= 0 ; --i){
            strides[i] = eleSize;
            eleSize *= shapes[i];
            eleC *= shapes[i];
        }
        this->eleCount = eleC;
    }
};

struct Object{

    MemoryBlock mb;
    Class* clsInfo {nullptr};
    Object* super {nullptr};
    Scope* scope {nullptr};
    volatile int _ref {1};
    U32 flags {0};
    std::unique_ptr<ArrayDesc> arrayDesc;

    Object(Scope* scope, Class* clsInfo, CList<int> shapes);
    Object(){}
    ~Object();

    void ref(){
        h_atomic_add(&_ref, 1);
    }
    //fetch_add
    void unref();
    bool hasFlag(int flag)const{ (flags & flag) == flag;}
    void addFlag(int flag)const{ flags |= flag;}
    U32 dataSize();

    void freeData();
    void setStringAsData(CString buf);
};

}
