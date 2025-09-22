
#include <string.h>
#include <unordered_map>
#include "cons.h"
#include "common/c_common.h"
#include "common/common.h"
#include "utils/Arrays.hpp"
#include "utils/primitive_cmp.hpp"

#define H7L_PRIMITIVE_APPLY_VALUE(srcP, dstPtr, dstPriType)\
switch (dstPriType) {\
case kPriType_CHAR:\
case kPriType_BOOL:{\
    char* dstP = (char*)dstPtr;\
    *dstP = *srcP;\
}break;\
case kPriType_UCHAR:{\
    uint8* dstP = (uint8*)dstPtr;\
    *dstP = *srcP;\
}break;\
case kPriType_SHORT:{\
    sint16* dstP = (sint16*)dstPtr;\
    *dstP = *srcP;\
}break;\
case kPriType_USHORT:{\
    uint16* dstP = (uint16*)dstPtr;\
    *dstP = *srcP;\
}break;\
case kPriType_INT:{\
    sint32* dstP = (sint32*)dstPtr;\
    *dstP = *srcP;\
}break;\
case kPriType_UINT:{\
    uint32* dstP = (uint32*)dstPtr;\
    *dstP = *srcP;\
}break;\
case kPriType_LONG:{\
    sint64* dstP = (sint64*)dstPtr;\
    *dstP = *srcP;\
}break;\
case kPriType_ULONG:{\
    uint64* dstP = (uint64*)dstPtr;\
    *dstP = *srcP;\
}break;\
case kPriType_FLOAT:{\
    float* dstP = (float*)dstPtr;\
    *dstP = *srcP;\
}break;\
case kPriType_DOUBLE:{\
    double* dstP = (double*)dstPtr;\
    *dstP = *srcP;\
}break;\
}


#define H7L_PRIMITIVE_CMP_VALUE(srcP, dstPtr, dstPriType)\
switch (dstPriType) {\
case kPriType_CHAR:\
case kPriType_BOOL:{\
    char* dstP = (char*)dstPtr;\
    return safe_compare(*srcP, *dstP);\
}break;\
case kPriType_UCHAR:{\
    uint8* dstP = (uint8*)dstPtr;\
    return safe_compare(*srcP, *dstP);\
}break;\
case kPriType_SHORT:{\
    sint16* dstP = (sint16*)dstPtr;\
    return safe_compare(*srcP, *dstP);\
}break;\
case kPriType_USHORT:{\
    uint16* dstP = (uint16*)dstPtr;\
    return safe_compare(*srcP, *dstP);\
}break;\
case kPriType_INT:{\
    sint32* dstP = (sint32*)dstPtr;\
    return safe_compare(*srcP, *dstP);\
}break;\
case kPriType_UINT:{\
    uint32* dstP = (uint32*)dstPtr;\
    return safe_compare(*srcP, *dstP);\
}break;\
case kPriType_LONG:{\
    sint64* dstP = (sint64*)dstPtr;\
    return safe_compare(*srcP, *dstP);\
}break;\
case kPriType_ULONG:{\
    uint64* dstP = (uint64*)dstPtr;\
    return safe_compare(*srcP, *dstP);\
}break;\
case kPriType_FLOAT:{\
    float* dstP = (float*)dstPtr;\
    return safe_compare(*srcP, *dstP);\
}break;\
case kPriType_DOUBLE:{\
    double* dstP = (double*)dstPtr;\
    return safe_compare(*srcP, *dstP);\
}break;\
}

namespace h7l {

int primitive_get_size(int priType){
    static std::unordered_map<int,int> s_pri_sizeMap = {
        {kPriType_BOOL, sizeof(char)},
        {kPriType_CHAR, sizeof(char)},
        {kPriType_UCHAR, sizeof(unsigned char)},
        {kPriType_SHORT, sizeof(short)},
        {kPriType_USHORT, sizeof(unsigned short)},
        {kPriType_INT, sizeof(int)},
        {kPriType_UINT, sizeof(unsigned int)},
        {kPriType_LONG, sizeof(long long)},
        {kPriType_ULONG, sizeof(unsigned long long)},
        {kPriType_FLOAT, sizeof(float)},
        {kPriType_DOUBLE, sizeof(double)},
    };
    auto it = s_pri_sizeMap.find(priType);
    return it != s_pri_sizeMap.end() ? it->second : 0;
}

void primitive_cast(int srcPriType, int dstPriType,
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
//------------------------------
static inline int _primitive_upgrade(int priType){
    switch (priType) {
    case kPriType_CHAR:
    case kPriType_UCHAR:
    case kPriType_SHORT:
    case kPriType_USHORT:
    {
        return kPriType_INT;
    }
    case kPriType_INT:
    case kPriType_UINT:
    {
        return kPriType_LONG;
    }
    case kPriType_LONG:
    case kPriType_ULONG:
    {
        return kPriType_LONG;
    }

    case kPriType_FLOAT:
    case kPriType_DOUBLE:{
        return kPriType_DOUBLE;
    };
    }
    return kPriType_NONE;
}

int primitive_compare(int t1, int t2, void* srcPtr, void* dstPtr){
    if(t1 < 0 || t2 < 0 || t1 >= kPriType_MAX || t2 >= kPriType_MAX){
        return kCmpRet_ERROR_ARG;
    }
    if((t1 == kPriType_BOOL && t2 != kPriType_BOOL)
        && (t1 != kPriType_BOOL && t2 == kPriType_BOOL) ){
        return kCmpRet_ERROR_CANT_CMP;
    }

    switch (t1) {
    case kPriType_CHAR:
    case kPriType_BOOL:
    {
        char* srcP = (char*)srcPtr;
        H7L_PRIMITIVE_CMP_VALUE(srcP, dstPtr, t2);
    }break;

    case kPriType_UCHAR:{
        uint8* srcP = (uint8*)srcPtr;
        H7L_PRIMITIVE_CMP_VALUE(srcP, dstPtr, t2);
    }break;

    case kPriType_SHORT:{
        sint16* srcP = (sint16*)srcPtr;
        H7L_PRIMITIVE_CMP_VALUE(srcP, dstPtr, t2);
    }break;

    case kPriType_USHORT:
    {
        uint16* srcP = (uint16*)srcPtr;
        H7L_PRIMITIVE_CMP_VALUE(srcP, dstPtr, t2);
    }break;

    case kPriType_INT:
    {
        sint32* srcP = (sint32*)srcPtr;
        H7L_PRIMITIVE_CMP_VALUE(srcP, dstPtr, t2);
    }break;
    case kPriType_UINT:
    {
        uint32* srcP = (uint32*)srcPtr;
        H7L_PRIMITIVE_CMP_VALUE(srcP, dstPtr, t2);
    }break;

    case kPriType_LONG:
    {
        sint64* srcP = (sint64*)srcPtr;
        H7L_PRIMITIVE_CMP_VALUE(srcP, dstPtr, t2);
    }break;
    case kPriType_ULONG:
    {
        uint64* srcP = (uint64*)srcPtr;
        H7L_PRIMITIVE_CMP_VALUE(srcP, dstPtr, t2);
    }break;

    case kPriType_FLOAT:
    {
        float* srcP = (float*)srcPtr;
        H7L_PRIMITIVE_CMP_VALUE(srcP, dstPtr, t2);
    }break;

    case kPriType_DOUBLE:
    {
        double* srcP = (double*)srcPtr;
        H7L_PRIMITIVE_CMP_VALUE(srcP, dstPtr, t2);
    }break;

    default:
        MED_ASSERT_X(false, "wrong priType = " << t2);
    }
    return kCmpRet_ERROR_ARG;
}
}
