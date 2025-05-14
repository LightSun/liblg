#pragma once

#include <memory.h>
#include "utils/h_atomic.h"
#include "common/common.h"
#include "src/cons.h"
//#include "src/lang/Allocator.h"

#define _H7L_MEM_GET_PRI_FUNC(T, MT)\
    T get##MT(T defVal = -1){\
        T data;\
        int typeSize;\
        if(getPrimitiveValue(&data, &typeSize) && typeSize == sizeof(T)){\
            return data;\
        }\
        return defVal;\
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
    void* data {nullptr};
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

    _H7L_MEM_GET_PRI_FUNC(char, Char)
    _H7L_MEM_GET_PRI_FUNC(unsigned char, UChar)
    _H7L_MEM_GET_PRI_FUNC(short, Short)
    _H7L_MEM_GET_PRI_FUNC(unsigned short, UShort)
    _H7L_MEM_GET_PRI_FUNC(int, Int)
    _H7L_MEM_GET_PRI_FUNC(unsigned int, UInt)
    _H7L_MEM_GET_PRI_FUNC(long long, Long)
    _H7L_MEM_GET_PRI_FUNC(unsigned long long, ULong)
    _H7L_MEM_GET_PRI_FUNC(float, Float)
    _H7L_MEM_GET_PRI_FUNC(double, Double)

    bool getBool(bool defVal = false){
        return getChar(defVal) != 0;
    }
    void initWithShareData(ShareData* sd, int _len, int _alignSize);
    void initWithStructSize(size_t structSize);
    //may need freeData() before.
    void reset(){
        memset(this, 0, sizeof(MemoryBlock));
    }

private:
    void initWithWrapPrimitive0(int priType, void* ptr, void* _dstPtr);
    static void castPrimitiveValue(int srcPriType, int dstPriType, void* srcPtr, void* dstPtr);
};
}
