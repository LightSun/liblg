#pragma once

#include "runtime/IObjectType.h"
#include "runtime/Instruction.h"
#include "runtime/utils/primitive_cmp.hpp"
#include "runtime/utils/Numbers.hpp"

namespace h7l { namespace runtime {

using uint8 = unsigned char;
using sint16 = signed short;
using uint16 = unsigned short;
using sint32 = signed int;
using uint32 = unsigned int;
using sint64 = Long;
using uint64 = ULong;
}}

#define H7L_PRIMITIVE_APPLY_VALUE(srcP, dstPtr, dstPriType)\
switch (dstPriType) {\
    case kType_S8:\
    case kType_BOOL:{\
        char* dstP = (char*)dstPtr;\
        *dstP = *srcP;\
}break;\
    case kType_U8:{\
        uint8* dstP = (uint8*)dstPtr;\
        *dstP = *srcP;\
}break;\
    case kType_S16:{\
        sint16* dstP = (sint16*)dstPtr;\
        *dstP = *srcP;\
}break;\
    case kType_U16:{\
        uint16* dstP = (uint16*)dstPtr;\
        *dstP = *srcP;\
}break;\
    case kType_S32:{\
        sint32* dstP = (sint32*)dstPtr;\
        *dstP = *srcP;\
}break;\
    case kType_U32:{\
        uint32* dstP = (uint32*)dstPtr;\
        *dstP = *srcP;\
}break;\
    case kType_S64:{\
        sint64* dstP = (sint64*)dstPtr;\
        *dstP = *srcP;\
}break;\
    case kType_U64:{\
        uint64* dstP = (uint64*)dstPtr;\
        *dstP = *srcP;\
}break;\
    case kType_FLOAT:{\
        float* dstP = (float*)dstPtr;\
        *dstP = *srcP;\
}break;\
    case kType_DOUBLE:{\
        double* dstP = (double*)dstPtr;\
        *dstP = *srcP;\
}break;\
}

#define H7L_PRIMITIVE_CMP_VALUE(srcP, dstPtr, dstPriType)\
switch (dstPriType) {\
    case kType_S8:\
    case kType_BOOL:{\
        char* dstP = (char*)dstPtr;\
        return safe_compare(*srcP, *dstP);\
}break;\
    case kType_U8:{\
        uint8* dstP = (uint8*)dstPtr;\
        return safe_compare(*srcP, *dstP);\
}break;\
    case kType_S16:{\
        sint16* dstP = (sint16*)dstPtr;\
        return safe_compare(*srcP, *dstP);\
}break;\
    case kType_U16:{\
        uint16* dstP = (uint16*)dstPtr;\
        return safe_compare(*srcP, *dstP);\
}break;\
    case kType_S32:{\
        sint32* dstP = (sint32*)dstPtr;\
        return safe_compare(*srcP, *dstP);\
}break;\
    case kType_U32:{\
        uint32* dstP = (uint32*)dstPtr;\
        return safe_compare(*srcP, *dstP);\
}break;\
    case kType_S64:{\
        sint64* dstP = (sint64*)dstPtr;\
        return safe_compare(*srcP, *dstP);\
}break;\
    case kType_U64:{\
        uint64* dstP = (uint64*)dstPtr;\
        return safe_compare(*srcP, *dstP);\
}break;\
    case kType_FLOAT:{\
        float* dstP = (float*)dstPtr;\
        return safe_compare(*srcP, *dstP);\
}break;\
    case kType_DOUBLE:{\
        double* dstP = (double*)dstPtr;\
        return safe_compare(*srcP, *dstP);\
}break;\
}

namespace h7l { namespace runtime {

static inline String opcode2Str(int opcode){
    switch (opcode) {
    case LOADK:{return "LOADK";}
    case LOADBOOL:{return "LOADBOOL";}
    case MOVE:{return "MOVE";}
    case ADD:{return "ADD";}
    case SUB:{return "SUB";}
    case MUL:{return "MUL";}
    case DIV:{return "DIV";}
    case CONCAT:{return "CONCAT";}

    case CALL:{return "CALL";}
    case RETURN:{return "RETURN";}

    case NEWLABEL:{return "NEW_LAB";}
    case JMPLABEL:{return "JMP_LAB";}
    case JMPLABEL_IF:{return "JMP_LAB_IF";}
    case JMPLABEL_NOT:{return "JMP_LAB_NOT";}

    case EQ:{return "EQ";}
    case LE:{return "LE";}
    case LT:{return "LT";}

    case NEWTABLE:{return "NEWTABLE";}
    case SETTABLE:{return "SETTABLE";}
    case CLOSURE:{return "CLOSURE";}
    case GETUPVAL:{return "GETUPVAL";}
    case SETUPVAL:{return "SETUPVAL";}

    case NEWARRAY:{return "NEWARRAY";}
    case GETARRAY:{return "GETARRAY";}
    case SETARRAY:{return "SETARRAY";}

    case CALL_C:{return "CALL_C";}
    case PRINT:{return "PRINT";}
    case HALT:{return "HALT";}


    default:
        MED_ASSERT_X(false, "opcode2Str >> support opcode = " << opcode);
    }
    return "";
}

static inline bool pri_isIntLike(int type){
    switch (type) {
    case kType_S8:
    case kType_U8:
    case kType_S16:
    case kType_U16:
    case kType_S32:
    case kType_U32:
    case kType_S64:
    case kType_U64:
        return true;
    }
    return false;
}

static inline Long pri_getAsLong(int type, const TypeValue& base){
    switch (type) {
    case kType_S8:
        return base.s8;
    case kType_U8:
        return base.u8;
    case kType_S16:
        return base.s16;
    case kType_U16:
        return base.u16;
    case kType_S32:
        return base.s32;
    case kType_U32:
        return base.u32;
    case kType_S64:
        return base.s64;
    case kType_U64:
        return base.u64;
    }
    MED_ASSERT_X(false, "not long-like number.");
    return 0;
}

static inline int pri_getAsInt(int type, const TypeValue& base){
    switch (type) {
    case kType_S8:
        return base.s8;
    case kType_U8:
        return base.u8;
    case kType_S16:
        return base.s16;
    case kType_U16:
        return base.u16;
    case kType_S32:
        return base.s32;
    case kType_U32:
        return base.u32;
    case kType_S64:
        return base.s64;
    case kType_U64:
        return base.u64;
    }
    MED_ASSERT_X(false, "not int-like number.");
    return 0;
}

static inline double pri_getAsNumber(int type, const TypeValue& base){
    switch (type) {
    case kType_S8:
        return base.s8;
    case kType_U8:
        return base.u8;
    case kType_S16:
        return base.s16;
    case kType_U16:
        return base.u16;
    case kType_S32:
        return base.s32;
    case kType_U32:
        return base.u32;
    case kType_S64:
        return base.s64;
    case kType_U64:
        return base.u64;

    case kType_FLOAT:
        return base.f32;

    case kType_DOUBLE:
        return base.f64;
    }
    MED_ASSERT_X(false, "not number-like.");
    return 0;
}

//in windows has-ret-func must has return case. or-else may cause illegal instruction.
static inline void val_printTo(int type, TypeValue& base, std::stringstream& ss){
    switch (type) {
    case kType_S8:
    {
        ss << base.s8;
    }break;
    case kType_U8:
    {
        ss << base.u8;
    }break;
    case kType_S16:
    {
        ss << base.s16;
    }break;
    case kType_U16:
    {
        ss << base.u16;
    }break;
    case kType_S32:
    {
        ss << base.s32;
    }break;
    case kType_U32:
    {
        ss << base.u32;
    }break;
    case kType_S64:
    {
        ss << base.s64;
    }break;
    case kType_U64:
    {
        ss << base.u64;
    }break;

    case kType_FLOAT:
    {
        ss << base.f32;
    }break;

    case kType_DOUBLE:
    {
        ss << base.f64;
    }break;

    case kType_BOOL:
    {
        ss << base.b;
    }break;

    case kType_NULL:
    {
        ss << "nullptr";
    }break;

    case kType_VOID:
    {
        ss << "void";
    }break;

    default:
    {
        base.ptr->printTo(ss);
    }break;
    }
}

static inline int pri_size(int type){
    switch (type) {
    case kType_BOOL:
    case kType_S8:
    case kType_U8:
        return sizeof(char);
    case kType_S16:
    case kType_U16:
        return sizeof(short);
    case kType_S32:
    case kType_U32:
        return sizeof(int);
    case kType_S64:
    case kType_U64:
        return sizeof(Long);

    case kType_FLOAT:
        return sizeof(float);

    case kType_DOUBLE:
        return sizeof(double);

    case kType_VOID:
        return 0;

    default:
        return sizeof(void*);
    }
}

static inline bool pri_is_base_type(int type){
    switch (type) {
    case kType_BOOL:
    case kType_S8:
    case kType_U8:
    case kType_S16:
    case kType_U16:
    case kType_S32:
    case kType_U32:
    case kType_S64:
    case kType_U64:
    case kType_FLOAT:
    case kType_DOUBLE:
        return true;

    default:
        return false;
    }
}

static inline int pri_base_type_cmp(int type, int t2, void* srcPtr, void* dstPtr){
    switch (type) {
    case kType_BOOL:{
        char* srcP = (char*)srcPtr;
        H7L_PRIMITIVE_CMP_VALUE(srcP, dstPtr, t2);
    }break;

    case kType_S8:{
        char* srcP = (char*)srcPtr;
        H7L_PRIMITIVE_CMP_VALUE(srcP, dstPtr, t2);
    }break;
    case kType_U8:{
        uint8* srcP = (uint8*)srcPtr;
        H7L_PRIMITIVE_CMP_VALUE(srcP, dstPtr, t2);
    }break;
    case kType_S16:{
        sint16* srcP = (sint16*)srcPtr;
        H7L_PRIMITIVE_CMP_VALUE(srcP, dstPtr, t2);
    }break;
    case kType_U16:{
        uint16* srcP = (uint16*)srcPtr;
        H7L_PRIMITIVE_CMP_VALUE(srcP, dstPtr, t2);
    }break;
    case kType_S32:{
        sint32* srcP = (sint32*)srcPtr;
        H7L_PRIMITIVE_CMP_VALUE(srcP, dstPtr, t2);
    }break;
    case kType_U32:{
        uint32* srcP = (uint32*)srcPtr;
        H7L_PRIMITIVE_CMP_VALUE(srcP, dstPtr, t2);
    }break;
    case kType_S64:{
        sint64* srcP = (sint64*)srcPtr;
        H7L_PRIMITIVE_CMP_VALUE(srcP, dstPtr, t2);
    }break;
    case kType_U64:{
        uint64* srcP = (uint64*)srcPtr;
        H7L_PRIMITIVE_CMP_VALUE(srcP, dstPtr, t2);
    }break;
    case kType_FLOAT:{
        float* srcP = (float*)srcPtr;
        H7L_PRIMITIVE_CMP_VALUE(srcP, dstPtr, t2);
    }break;
    case kType_DOUBLE:{
        double* srcP = (double*)srcPtr;
        H7L_PRIMITIVE_CMP_VALUE(srcP, dstPtr, t2);
    }break;
    }
    return -1;
}

static inline bool pri_base_type_array_eqs(int type, char* p11, char* p12, size_t cnt){
    switch (type) {
    case kType_BOOL:{
        char* p1 = (char*)p11;
        char* p2 = (char*)p12;
        for(size_t i = 0 ; i < cnt ; ++i){
            if(p1[i] != p2[i]){
                return false;
            }
        }
        return true;
    }break;

    case kType_S8:{
        char* p1 = (char*)p11;
        char* p2 = (char*)p12;
        for(size_t i = 0 ; i < cnt ; ++i){
            if(p1[i] != p2[i]){
                return false;
            }
        }
        return true;
    }break;
    case kType_U8:{
        uint8* p1 = (uint8*)p11;
        uint8* p2 = (uint8*)p12;
        for(size_t i = 0 ; i < cnt ; ++i){
            if(p1[i] != p2[i]){
                return false;
            }
        }
        return true;
    }break;
    case kType_S16:{
        short* p1 = (short*)p11;
        short* p2 = (short*)p12;
        for(size_t i = 0 ; i < cnt ; ++i){
            if(p1[i] != p2[i]){
                return false;
            }
        }
        return true;
    }break;
    case kType_U16:{
        uint16* p1 = (uint16*)p11;
        uint16* p2 = (uint16*)p12;
        for(size_t i = 0 ; i < cnt ; ++i){
            if(p1[i] != p2[i]){
                return false;
            }
        }
        return true;
    }break;
    case kType_S32:{
        int* p1 = (int*)p11;
        int* p2 = (int*)p12;
        for(size_t i = 0 ; i < cnt ; ++i){
            if(p1[i] != p2[i]){
                return false;
            }
        }
        return true;
    }break;
    case kType_U32:{
        uint32* p1 = (uint32*)p11;
        uint32* p2 = (uint32*)p12;
        for(size_t i = 0 ; i < cnt ; ++i){
            if(p1[i] != p2[i]){
                return false;
            }
        }
        return true;
    }break;
    case kType_S64:{
        Long* p1 = (Long*)p11;
        Long* p2 = (Long*)p12;
        for(size_t i = 0 ; i < cnt ; ++i){
            if(p1[i] != p2[i]){
                return false;
            }
        }
        return true;
    }break;
    case kType_U64:{
        ULong* p1 = (ULong*)p11;
        ULong* p2 = (ULong*)p12;
        for(size_t i = 0 ; i < cnt ; ++i){
            if(p1[i] != p2[i]){
                return false;
            }
        }
        return true;
    }break;
    case kType_FLOAT:{
        float* p1 = (float*)p11;
        float* p2 = (float*)p12;
        for(size_t i = 0 ; i < cnt ; ++i){
            if(safe_compare(p1[i], p2[i]) != 0){
                return false;
            }
        }
        return true;
    }break;
    case kType_DOUBLE:{
        double* p1 = (double*)p11;
        double* p2 = (double*)p12;
        for(size_t i = 0 ; i < cnt ; ++i){
            if(safe_compare(p1[i], p2[i]) != 0){
                return false;
            }
        }
        return true;
    }break;
    }

    return false;
}

static inline void pri_base_type_array_print(std::stringstream& ss, int type, char* p11, int cnt){
#define DEF_PRINT(ktype)\
    ktype* p1 = (ktype*)p11;\
    for(size_t i = 0 ; i < cnt ; ++i){\
        ss << p1[i];\
        if(i != cnt -1){\
            ss << ",";\
        }\
    }

    switch (type) {
    case kType_BOOL:
    case kType_S8:{
        DEF_PRINT(char);
    }break;
    case kType_U8:{
        DEF_PRINT(uint8);
    }break;
    case kType_S16:{
        DEF_PRINT(sint16);
    }break;
    case kType_U16:{
        DEF_PRINT(uint16);
    }break;
    case kType_S32:{
        DEF_PRINT(sint32);
    }break;
    case kType_U32:{
        DEF_PRINT(uint32);
    }break;
    case kType_S64:{
        DEF_PRINT(sint64);
    }break;
    case kType_U64:{
        DEF_PRINT(uint64);
    }break;
    case kType_FLOAT:{
        DEF_PRINT(float);
    }break;
    case kType_DOUBLE:{
        DEF_PRINT(double);
    }break;
    }
#undef DEF_PRINT
}

static inline void pri_cast(int srcPriType, int dstPriType, void* srcPtr, void* dstPtr){
    switch (srcPriType) {
    case kType_S8:
    case kType_BOOL:
    {
        char* srcP = (char*)srcPtr;
        H7L_PRIMITIVE_APPLY_VALUE(srcP, dstPtr, dstPriType);
    }break;

    case kType_U8:{
        uint8* srcP = (uint8*)srcPtr;
        H7L_PRIMITIVE_APPLY_VALUE(srcP, dstPtr, dstPriType);
    }break;

    case kType_S16:{
        sint16* srcP = (sint16*)srcPtr;
        H7L_PRIMITIVE_APPLY_VALUE(srcP, dstPtr, dstPriType);
    }break;

    case kType_U16:
    {
        uint16* srcP = (uint16*)srcPtr;
        H7L_PRIMITIVE_APPLY_VALUE(srcP, dstPtr, dstPriType);
    }break;

    case kType_S32:
    {
        sint32* srcP = (sint32*)srcPtr;
        H7L_PRIMITIVE_APPLY_VALUE(srcP, dstPtr, dstPriType);
    }break;
    case kType_U32:
    {
        uint32* srcP = (uint32*)srcPtr;
        H7L_PRIMITIVE_APPLY_VALUE(srcP, dstPtr, dstPriType);
    }break;

    case kType_S64:
    {
        sint64* srcP = (sint64*)srcPtr;
        H7L_PRIMITIVE_APPLY_VALUE(srcP, dstPtr, dstPriType);
    }break;
    case kType_U64:
    {
        uint64* srcP = (uint64*)srcPtr;
        H7L_PRIMITIVE_APPLY_VALUE(srcP, dstPtr, dstPriType);
    }break;

    case kType_FLOAT:
    {
        float* srcP = (float*)srcPtr;
        H7L_PRIMITIVE_APPLY_VALUE(srcP, dstPtr, dstPriType);
    }break;

    case kType_DOUBLE:
    {
        double* srcP = (double*)srcPtr;
        H7L_PRIMITIVE_APPLY_VALUE(srcP, dstPtr, dstPriType);
    }break;

    default:
        MED_ASSERT_X(false, "wrong priType = " << srcPriType);
    }
}

template<typename T>
static inline int hashCode0(T var0) {
    return (int)(var0 ^ unsignedShiftLong(var0, 32));
}
static inline int floatToIntBits_wrap(float var0) {
    int var1 = floatToRawIntBits(var0);
    if ((var1 & 2139095040) == 2139095040 && (var1 & 8388607) != 0) {
        var1 = 2143289344;
    }

    return var1;
}

static inline int base_hash(int type, void* ptr){
    switch (type) {
    case kType_S8:
    {
        return *(char*)ptr;
    }break;
    case kType_U8:
    {
        return *(unsigned char*)ptr;
    }break;
    case kType_S16:
    {
        return *(short*)ptr;
    }break;
    case kType_U16:
    {
        return *(unsigned short*)ptr;
    }break;
    case kType_S32:
    {
        return *(int*)ptr;
    }break;
    case kType_U32:
    {
        return *(unsigned int*)ptr;
    }break;
    case kType_S64:
    {
        return hashCode0<Long>(*(Long*)ptr);
    }break;
    case kType_U64:
    {
        return hashCode0<ULong>(*(ULong*)ptr);
    }break;

    case kType_FLOAT:
    {
        return floatToIntBits_wrap(*(float*)ptr);
    }break;

    case kType_DOUBLE:
    {
        Long val = doubleToLongBits(*(double*)ptr);
        return hashCode0(val);
    }break;

    case kType_BOOL:
    {
        return (*(char*)ptr) !=0 ? 1231 : 1237;//like java boolean
    }break;

    case kType_NULL:
    case kType_VOID:
    default:
    {
        return 0;
    }break;
    }
}

static inline int str_hash(const char* data, size_t len){
    int var1 = 0;
    if (len > 0) {
        for(size_t i = 0 ; i < len ; ++i){
            var1 = 31 * var1 + data[i];
        }
    }
    return var1;
}


}}
