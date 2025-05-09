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
};

enum FuncOp{
    kFuncOp_add,
    kFuncOp_sub,
    kFuncOp_multi,
    kFuncOp_divide,
    kFuncOp_mod,
    kFuncOp_copy,
    kFuncOp_deepCopy,
};

int primitive_get_size(int priType);

}
