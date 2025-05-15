
#include <string.h>
#include <unordered_map>
#include "cons.h"
#include "common/c_common.h"
#include "common/common.h"
#include "utils/Arrays.hpp"

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
static bool _primitive_int_like_equals(int t1, int t2, void* ptr1, void* ptr2){
    auto size1 = primitive_get_size(t1);
    auto size2 = primitive_get_size(t2);
    //
    if(primitive_isSigned(t1)){
        if(size1 > size2){
            //t2 -> t1
            ArrayHolder holder(size1);
            primitive_cast(t2, t1, ptr2, holder.ptr);
            return primitive_compare(t1, ptr1, holder.ptr) == kCmpRet_EQUALS;
        }else if(size1 == size2){
            if(primitive_isSigned(t2)){
                //can't reach here.
                MED_ASSERT_X(false, "_primitive_int_like_equals >> can't reach 1");
                return false;
            }
            auto nt1 = _primitive_upgrade(t1);
            char buf[8];
        }else{

        }
    }else{

    }
    //
}

bool primitive_equals(int t1, int t2, void* ptr1, void* ptr2){
    if(t1 > kPriType_MAX || t2 > kPriType_MAX){
        MED_ASSERT_X(false, "primitive_equals >> can't reach");
        return false;
    }
    if((t1 == kPriType_BOOL && t2 != kPriType_BOOL)
        && (t1 != kPriType_BOOL && t2 == kPriType_BOOL) ){
        fprintf(stderr, "primitive_equals >> bool can't cmp to non-bool.\n");
        return false;
    }
    if(t1 == t2){
        if(primitive_isIntLike(t1)){
            return memcmp(ptr1, ptr2, primitive_get_size(t1)) == 0;
        }else if(t1 == kPriType_FLOAT){
            float* d1 = (float*)ptr1;
            float* d2 = (float*)ptr2;
            return IsAlmostEqual(*d1, *d2);
        }else{
            double* d1 = (double*)ptr1;
            double* d2 = (double*)ptr2;
            return IsAlmostEqual(*d1, *d2);
        }
    }
    if(primitive_isIntLike(t1)){
        if(primitive_isIntLike(t2)){
            return _primitive_int_like_equals(t1, t2, ptr1, ptr2);
        }else if(t2 == kPriType_FLOAT){
            auto size1 = primitive_get_size(t1);
            const int size2 = sizeof(float);
            if(size1 > size2 || t1 == kPriType_UINT){
                //to double
                double d1 = 0;
                double d2 = 0;
                primitive_cast(t1, kPriType_DOUBLE, ptr1, &d1);
                primitive_cast(t2, kPriType_DOUBLE, ptr2, &d2);
                return IsAlmostEqual(d1, d2);
            }else{
                //t1 -> float
                float* d2 = (float*)ptr2;
                float val1 = 0;
                primitive_cast(t1, kPriType_FLOAT, ptr1, &val1);
                return IsAlmostEqual(val1, *d2);
            }
        }else{
            //t2 is double, t1 -> double
            double* d2 = (double*)ptr2;
            double val1 = 0;
            primitive_cast(t1, t2, ptr1, &val1);
            return IsAlmostEqual(val1, *d2);
        }
    }else if(t1 == kPriType_FLOAT){
        const int size1 = sizeof(float);
        auto size2 = primitive_get_size(t2);
        if(primitive_isIntLike(t2)){
            if(size2 > size1 || t2 == kPriType_UINT){
                double d1 = 0;
                double d2 = 0;
                primitive_cast(t1, kPriType_DOUBLE, ptr1, &d1);
                primitive_cast(t2, kPriType_DOUBLE, ptr2, &d2);
                return IsAlmostEqual(d1, d2);
            }else{
                //t2 -> float
                float* d1 = (float*)ptr1;
                float val2 = 0;
                primitive_cast(t2, kPriType_FLOAT, ptr2, &val2);
                return IsAlmostEqual(*d1, val2);
            }
        }else{
            //t2 is double
            double* d2 = (double*)ptr2;
            double d1 = 0;
            primitive_cast(t1, kPriType_DOUBLE, ptr1, &d1);
            return IsAlmostEqual(d1, *d2);
        }
    }else{
        //t1 is double
        double* d1 = (double*)ptr1;
        double d2 = 0;
        primitive_cast(t2, kPriType_DOUBLE, ptr2, &d2);
        return IsAlmostEqual(*d1, d2);
    }
}

int primitive_compare(int priType, void* srcPtr, void* dstPtr){

}

}
