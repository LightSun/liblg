#pragma once

#define TYPE_ID_INT "int"
#define TYPE_ID_CHAR "char"
#define TYPE_ID_SHORT "short"
#define TYPE_ID_LONG "long"
#define TYPE_ID_FLOAT "float"
#define TYPE_ID_DOUBLE "double"

#define H7L_CLASS_NAME_RC "h7l.RuntimeContext"
#define H7L_CLASS_NAME_VM "h7l.VM"

namespace h7l {

enum MemoryFlag{
    kMemFlag_FREE                = 0x0001,
    kMemFlag_SHARE               = 0x0002,
    kMemFlag_WRAP_PRIMITIVE      = 0x0004,
    kMemFlag_WRAP_PRIMITIVE_PTR  = 0x0008,
};

enum PrimiveType{
    kPriType_NONE = -1,
    kPriType_CHAR,
    kPriType_UCHAR,
    kPriType_SHORT,
    kPriType_USHORT,
    kPriType_INT,
    kPriType_UINT,
    kPriType_LONG,
    kPriType_ULONG,
    kPriType_BOOL,
    kPriType_FLOAT,
    kPriType_DOUBLE,
    kPriType_MAX,
};

enum FuncOp{
    kFuncOp_add,
    kFuncOp_sub,
    kFuncOp_multi,
    kFuncOp_divide,
    kFuncOp_mod,
    kFuncOp_copy,
    kFuncOp_deepCopy,
    kFuncOp_compare,
};

enum CompareResult{
    kCmpRet_ERROR_ARG    = -4,   //arg error
    kCmpRet_ERROR_CANT_CMP = -3, //like bool can't cmp to non-bool, primitive cmp to object, t1 and t2 has no extend
    kCmpRet_NAN  = -2,           //has NAN
    kCmpRet_LESS = -1,
    kCmpRet_EQUALS,
    kCmpRet_GREATER
};

//0 for non-primitive
int primitive_get_size(int priType);
void primitive_cast(int srcPriType, int dstPriType, void* srcPtr, void* dstPtr);
int primitive_compare(int srcPriType, int dstPriType, void* srcPtr, void* dstPtr);

static inline bool primitive_isIntLike(int priType){
    switch (priType) {
    case kPriType_CHAR:
    case kPriType_UCHAR:
    case kPriType_SHORT:
    case kPriType_USHORT:
    case kPriType_INT:
    case kPriType_UINT:
    case kPriType_LONG:
    case kPriType_ULONG:
    case kPriType_BOOL:
        return true;
    }
    return false;
}
static inline bool primitive_isFloatLike(int priType){
    return priType == kPriType_FLOAT || priType == kPriType_DOUBLE;
}

static inline bool primitive_isSigned(int priType){
    switch (priType) {
    case kPriType_CHAR:
    case kPriType_SHORT:
    case kPriType_INT:
    case kPriType_LONG:
        return true;
    }
    return false;
}

}
