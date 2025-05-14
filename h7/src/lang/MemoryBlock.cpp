#include <memory.h>
#include <stddef.h>
#include "common/c_common.h"
#include "src/lang/MemoryBlock.h"
#include "src/lang/macro.h"
#include "src/cons.h"

using namespace h7l;

void ShareData::unref(){
    if(h_atomic_add(&_ref, -1) == 1){
        if(data){
#ifdef H7L_DBG_ShareData_DELETE
            printf("ShareData: is delete now.\n");
#endif
            free(data);
            data = nullptr;
        }
        delete (this);
    }
}

void MemoryBlock::initWithShareData(ShareData* sd, int _len, int _alignSize){
    data = sd;
    len = _len;
    cap = _alignSize;
    offset = 0;
    addFlag(kMemFlag_SHARE);
}
void MemoryBlock::initWithStructSize(size_t structSize){
    data = malloc(structSize);
    len = structSize;
    cap = structSize;
    offset = 0;
    addFlag(kMemFlag_FREE);
}

void* MemoryBlock::getDataPtr(){
    if(hasFlag(kMemFlag_WRAP_PRIMITIVE)){
        char* dstPtr = (char*)this + offsetof(MemoryBlock, len);
        return dstPtr;
    }
    //kMemFlag_WRAP_PRIMITIVE_PTR
    if(data){
        if(hasFlag(kMemFlag_SHARE)){
            ShareData* sd = (ShareData*)data;
            return sd->data;
        }
        return data;
    }
    return nullptr;
}
void MemoryBlock::freeData(){
    //kMemFlag_WRAP_PRIMITIVE_PTR may with share
    if(hasFlag(kMemFlag_WRAP_PRIMITIVE)){
        return;
    }
    if(data){
        if(hasFlag(kMemFlag_FREE)){
            free(data);
        }else if(hasFlag(kMemFlag_SHARE)){
            ShareData* sd = (ShareData*)data;
            sd->unref();
        }
        data = nullptr;
    }
}
void MemoryBlock::initWithWrapPrimitive(int priType, void* ptr){
    char* dstPtr = (char*)this + offsetof(MemoryBlock, len);
    initWithWrapPrimitive0(priType, ptr, dstPtr);
}
void MemoryBlock::initWithWrapPrimitiveWithRawAddr(int priType, void* ptr){
    char* dstPtr = (char*)getDataPtr();
    initWithWrapPrimitive0(priType, ptr, dstPtr);
}
void MemoryBlock::initWithWrapPrimitiveWithRawAddr2(int priType, void* srcPtr){
    char* dstPtr = (char*)getDataPtr();
    castPrimitiveValue(offset, priType, srcPtr, dstPtr);
    offset = priType;
    flags = kMemFlag_WRAP_PRIMITIVE;
}
void MemoryBlock::initWithWrapPrimitive2(int priType, void* srcPtr){
    char* dstPtr = (char*)this + offsetof(MemoryBlock, len);
    castPrimitiveValue(offset, priType, srcPtr, dstPtr);
    offset = priType;
    flags = kMemFlag_WRAP_PRIMITIVE;
}
void MemoryBlock::initWithWrapPrimitivePtr(int priType, void* ptr){
    data = ptr;
    offset = priType;
    flags = kMemFlag_WRAP_PRIMITIVE_PTR;
}
void MemoryBlock::initWithWrapPrimitivePtr(int priType, void* srcPtr, void* dstPtr){
    castPrimitiveValue(offset, priType, srcPtr, dstPtr);
    data = dstPtr;
    flags = kMemFlag_WRAP_PRIMITIVE_PTR;
    offset = priType;
}
void MemoryBlock::initWithWrapPrimitiveSharePtr(int priType, ShareData* ptr){
    data = ptr;
    offset = priType;
    flags = kMemFlag_WRAP_PRIMITIVE_PTR | kMemFlag_SHARE;
}
int MemoryBlock::getPrimitiveType()const{
    if(hasFlag(kMemFlag_WRAP_PRIMITIVE) || hasFlag(kMemFlag_WRAP_PRIMITIVE_PTR)){
        return offset;
    }
    return kPriType_NONE;
}
bool MemoryBlock::getPrimitiveValue(void* ptr, int* typeSize){
    if(hasFlag(kMemFlag_WRAP_PRIMITIVE)){
        char* dstPtr = (char*)this + offsetof(MemoryBlock, len);
        int size = primitive_get_size(offset);
        MED_ASSERT(size > 0);
        memcpy(ptr, dstPtr, size);
        if(typeSize){
            *typeSize = size;
        }
        return true;
    }else if(hasFlag(kMemFlag_WRAP_PRIMITIVE_PTR)){
        auto dp = getDataPtr();
        MED_ASSERT(dp);
        int size = primitive_get_size(offset);
        MED_ASSERT(size > 0);
        memcpy(ptr, dp, size);
        if(typeSize){
            *typeSize = size;
        }
        return true;
    }
    return false;
}
void MemoryBlock::setStringAsData(CString buf){
    if(hasFlag(kMemFlag_SHARE)){
        freeData();
    }
    int actLen = buf.length() + 1;
    int mod = actLen % 8;
    int cap = mod > 0 ? (actLen / 8 + 1) * 8 : actLen;
    if(data){
        data = realloc(data, cap);
    }else{
        data = malloc(cap);
    }
    memcpy(data, buf.data(), buf.length());
    ((char*)getDataPtr())[buf.length()] = '\0';
    this->len = buf.length();
    this->cap = cap;
    this->offset = 0;
    this->flags = kMemFlag_FREE;
}
//----------------------------------
//----------------------------------
void MemoryBlock::initWithWrapPrimitive0(int priType, void* ptr, void* _dstPtr){
    char* dstPtr = (char*)_dstPtr;
    switch (priType) {
    case kPriType_CHAR:
    case kPriType_UCHAR:
    case kPriType_BOOL:
    {
        memcpy(dstPtr, ptr, sizeof(char));
    }break;

    case kPriType_SHORT:
    case kPriType_USHORT:
    {
        memcpy(dstPtr, ptr, sizeof(short));
    }break;

    case kPriType_INT:
    case kPriType_UINT:
    {
        memcpy(dstPtr, ptr, sizeof(int));
    }break;

    case kPriType_LONG:
    case kPriType_ULONG:
    {
        memcpy(dstPtr, ptr, sizeof(long long));
    }break;

    case kPriType_FLOAT:
    {
        memcpy(dstPtr, ptr, sizeof(float));
    }break;

    case kPriType_DOUBLE:
    {
        memcpy(dstPtr, ptr, sizeof(double));
    }break;

    default:
        MED_ASSERT_X(false, "wrong priType = " << priType);
    }
    offset = priType;
    flags = kMemFlag_WRAP_PRIMITIVE;
}
void MemoryBlock::castPrimitiveValue(int srcPriType, int dstPriType,
                                         void* srcPtr, void* dstPtr){
    switch (srcPriType) {
    case kPriType_CHAR:
    case kPriType_BOOL:
    {
        char* srcP = (char*)srcPtr;
        H7L_PRIMITIVE_APPLY_VALUE(srcP, dstPtr, dstPriType);
    }break;

    case kPriType_UCHAR:{
        uint8* srcP = (uint8*)srcPtr;
        H7L_PRIMITIVE_APPLY_VALUE(srcP, dstPtr, dstPriType);
    }break;

    case kPriType_SHORT:{
        sint16* srcP = (sint16*)srcPtr;
        H7L_PRIMITIVE_APPLY_VALUE(srcP, dstPtr, dstPriType);
    }break;

    case kPriType_USHORT:
    {
        uint16* srcP = (uint16*)srcPtr;
        H7L_PRIMITIVE_APPLY_VALUE(srcP, dstPtr, dstPriType);
    }break;

    case kPriType_INT:
    {
        sint32* srcP = (sint32*)srcPtr;
        H7L_PRIMITIVE_APPLY_VALUE(srcP, dstPtr, dstPriType);
    }break;
    case kPriType_UINT:
    {
        uint32* srcP = (uint32*)srcPtr;
        H7L_PRIMITIVE_APPLY_VALUE(srcP, dstPtr, dstPriType);
    }break;

    case kPriType_LONG:
    {
        sint64* srcP = (sint64*)srcPtr;
        H7L_PRIMITIVE_APPLY_VALUE(srcP, dstPtr, dstPriType);
    }break;
    case kPriType_ULONG:
    {
        uint64* srcP = (uint64*)srcPtr;
        H7L_PRIMITIVE_APPLY_VALUE(srcP, dstPtr, dstPriType);
    }break;

    case kPriType_FLOAT:
    {
        float* srcP = (float*)srcPtr;
        H7L_PRIMITIVE_APPLY_VALUE(srcP, dstPtr, dstPriType);
    }break;

    case kPriType_DOUBLE:
    {
        double* srcP = (double*)srcPtr;
        H7L_PRIMITIVE_APPLY_VALUE(srcP, dstPtr, dstPriType);
    }break;

    default:
        MED_ASSERT_X(false, "wrong priType = " << srcPriType);
    }
}
