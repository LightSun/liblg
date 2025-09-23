#pragma once

#include "MemoryBlock.h"
#include "Value.h"
#include "macro.h"
#include "common/c_common.h"

namespace h7l {

struct Class;
struct Allocator;
class Scope;

struct ArrayDesc{
    List<int> shapes;
    List<int> strides;
    U32 baseSize {0};
    U32 eleCount {0}; //total

    ArrayDesc(U32 s): baseSize(s){}

    void setShape(CList<int> shapes){
        if(!shapes.empty()){
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
        }else{
            this->shapes.clear();
            strides.clear();
            eleCount = 0;
        }
    }
    bool toSubArray(int index, U32& offset, ArrayDesc& outArr){
        if(shapes.empty() || shapes[0] < index){
            return false;
        }
        int c = shapes[0];
        if(index < 0){
            index = c + index;
        }
        offset = index * strides[0];
        List<int> newShapes;
        for(size_t i = 1 ; i < shapes.size() ; ++i){
            newShapes.push_back(shapes[i]);
        }
        outArr.setShape(newShapes);
        return true;
    }
    void copyTo(ArrayDesc* dst){
        *dst = *this;
    }

private:
    ArrayDesc(){}
};

struct Object{

    MemoryBlock mb;
    Scope* scope {nullptr};
    Type* type {nullptr};
    //Object* super {nullptr};
    volatile int _ref {1};
    U32 flags {0};
    std::shared_ptr<ArrayDesc> arrayDesc;

    //clsInfo: base-component-type
    Object(Scope* scope, int priType, CList<int> shapes = {});
    Object(Scope* scope, Type* clsInfo, CList<int> shapes = {});
    Object(Scope* scope, Type* clsInfo, std::shared_ptr<ArrayDesc> desc);
    Object(){}
    ~Object();

    static Object* newPrimitive(Scope* scope, int priType);

    void ref(){
        h_atomic_add(&_ref, 1);
    }
    int getRefCnt(){
        return h_atomic_get(&_ref);
    }
    //fetch_add
    void unref();

    bool hasFlag(U32 flag)const{ return (flags & flag) == flag;}
    void addFlag(U32 flag){ flags |= flag;}
    void* getDataPtr(){return mb.getDataPtr();}
    int getTotalElementCount()const{
        return arrayDesc ? arrayDesc->eleCount : 0;
    }
    U32 getDataSize();
    //exclude array
    U32 getBaseDataSize();
    bool isArray()const{return arrayDesc && arrayDesc->eleCount > 0;}
    void reset();

    Class* getClass()const{
        if(!type->isPrimetiveType()){
            return (Class*)type;
        }
        return nullptr;
    }
    Type* getType(){return type;}
    //
    void setStringAsData(CString buf){
        mb.setStringAsData(buf);
    }
    Object* subArray(int index);
    //may need reset
    H7L_DEF_OBJ_WRAP_BASE(mb)
    H7L_DEF_OBJ_WRAP_BASE_PTR(mb)

    //int-like --> int-like : checkSize enough
    //float-like --> float-like : checkSize enough
    //int -> float or float -> int : ptr -> non-ptr
    bool castPrimitive(int priType);
    //newPtr: data addr
    bool castPrimitiveTo(int priType, void* newPtr);

    //---------------------------
    int compare(Object& oth);

    bool equals(Object& oth){
        return compare(oth) == kCmpRet_EQUALS;
    }
    bool compareEQ(Object& oth){
        return compare(oth) == kCmpRet_EQUALS;
    }
    bool compareLE(Object& oth){
        auto ret = compare(oth);
        return ret == kCmpRet_EQUALS || ret == kCmpRet_LESS ;
    }
    bool compareLT(Object& oth){
        return compare(oth) == kCmpRet_LESS ;
    }
    bool compareGT(Object& oth){
        return compare(oth) == kCmpRet_GREATER;
    }
    bool compareGE(Object& oth){
        auto ret = compare(oth);
        return ret == kCmpRet_EQUALS || ret == kCmpRet_GREATER;
    }
    Object* copy();
    Object* copyDeep();

private:
    void init0(Scope* scope, Type* type, ShareData* sd,
               std::shared_ptr<ArrayDesc> desc);
    void unrefChildObject();
};

struct ObjectDeleter{
    void operator()(Object* obj)const{
        obj->unref();
    }
};
}
