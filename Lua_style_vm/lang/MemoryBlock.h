#pragma once

#include <memory.h>
#include "utils/h_atomic.h"
#include "common/common.h"
#include "common/c_common.h"
#include "cons.h"

#define _H7L_MEM_GET_PRI_FUNC(T, MT)\
    T get##MT(T defVal = -1){\
        T data;\
        int typeSize;\
        if(getPrimitiveValue(&data, &typeSize) && typeSize == sizeof(T)){\
            return data;\
        }\
        return defVal;\
    }

#define _H7L_MEM_SET_PRI_FUNC(T, MT)\
    void setPrimitiveValue(T val){\
        auto pt = getPrimitiveType();\
        if(pt >= 0){\
            auto dstPtr = getDataPtr();\
            primitive_cast(kPriType_##MT, pt, &val, dstPtr);\
        }\
    }

namespace h7l {

struct ShareData{
    void* data {nullptr};
    volatile int _ref {1};

    ~ShareData(){
        unref();
    }

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
    void unref();
};

struct MemoryBlock{
    void* data {nullptr}; //may be ShareData.
    U32 len {0};        //for string , exclude tail which is '\0'
    U32 cap {0};        //after align
    U32 offset {0};     //offset of share-data
    U32 flags {0};

    ~MemoryBlock(){freeData();}

    void* getDataPtr();

    String asString(){
        return String((char*)getDataPtr(), len);
    }
    bool hasFlag(U32 flag)const{ return (flags & flag) == flag;}
    void addFlag(U32 flag){ flags |= flag;}

    void setStringAsData(CString buf);
    void freeData();
    bool isSharedData()const{ return hasFlag(kMemFlag_SHARE);}

    //-1 for non-primitive
    int getPrimitiveType()const;
    int getPrimitiveSize()const{ return primitive_get_size(offset);}
    bool isPrimitivePtr()const {return hasFlag(kMemFlag_WRAP_PRIMITIVE_PTR);}
    //false for non-primitive
    bool getPrimitiveValue(void* ptr, int* typeSize = nullptr);
    //direct
    void initWithWrapPrimitive(int priType, void* ptr);
    void initWithWrapPrimitive2(int priType, void* srcPtr);
    //direct
    void initWithWrapPrimitiveWithRawAddr(int priType, void* ptr);
    void initWithWrapPrimitiveWithRawAddr2(int priType, void* ptr);
    void initWithWrapPrimitivePtr(int priType, void* ptr);
    void initWithWrapPrimitivePtr(int priType, void* srcPtr, void* dstPtr);
    void initWithWrapPrimitiveSharePtr(int priType, ShareData* ptr);

    bool castPrimitive(int priType);
    bool castPrimitiveTo(int priType, void* newPtr);
    //compare
    int comparePrimitive(MemoryBlock& oth);
    bool isPrimitiveEQ(MemoryBlock& oth){
        return comparePrimitive(oth) == kCmpRet_EQUALS;
    }
    bool isPrimitiveLE(MemoryBlock& oth){
        auto ret = comparePrimitive(oth);
        return ret == kCmpRet_EQUALS || ret == kCmpRet_LESS ;
    }
    bool isPrimitiveLT(MemoryBlock& oth){
        return comparePrimitive(oth) == kCmpRet_LESS ;
    }
    bool isPrimitiveGT(MemoryBlock& oth){
        return comparePrimitive(oth) == kCmpRet_GREATER;
    }
    bool isPrimitiveGE(MemoryBlock& oth){
        auto ret = comparePrimitive(oth);
        return ret == kCmpRet_EQUALS || ret == kCmpRet_GREATER;
    }

    _H7L_MEM_GET_PRI_FUNC(sint8, Char)
    _H7L_MEM_GET_PRI_FUNC(uint8, UChar)
    _H7L_MEM_GET_PRI_FUNC(sint16, Short)
    _H7L_MEM_GET_PRI_FUNC(uint16, UShort)
    _H7L_MEM_GET_PRI_FUNC(sint32, Int)
    _H7L_MEM_GET_PRI_FUNC(uint32, UInt)
    _H7L_MEM_GET_PRI_FUNC(sint64, Long)
    _H7L_MEM_GET_PRI_FUNC(uint64, ULong)
    _H7L_MEM_GET_PRI_FUNC(float, Float)
    _H7L_MEM_GET_PRI_FUNC(double, Double)

    bool getBool(bool defVal = false){
        return getChar(defVal) != 0;
    }
    _H7L_MEM_SET_PRI_FUNC(bool, BOOL)
    _H7L_MEM_SET_PRI_FUNC(sint8, CHAR)
    _H7L_MEM_SET_PRI_FUNC(uint8, UCHAR)
    _H7L_MEM_SET_PRI_FUNC(sint16, SHORT)
    _H7L_MEM_SET_PRI_FUNC(uint16, USHORT)
    _H7L_MEM_SET_PRI_FUNC(sint32, INT)
    _H7L_MEM_SET_PRI_FUNC(uint32, UINT)
    _H7L_MEM_SET_PRI_FUNC(sint64, LONG)
    _H7L_MEM_SET_PRI_FUNC(uint64, ULONG)
    _H7L_MEM_SET_PRI_FUNC(float, FLOAT)
    _H7L_MEM_SET_PRI_FUNC(double, DOUBLE)

    void initWithShareData(ShareData* sd, int _len, int _alignSize);
    void initWithStructSize(size_t structSize);
    //may need freeData() before.
    void reset(){
        memset(this, 0, sizeof(MemoryBlock));
    }
    void copyTo(MemoryBlock* other);
    //-----------------------------------------

private:
    void initWithWrapPrimitive0(int priType, void* ptr, void* _dstPtr);
};
}
