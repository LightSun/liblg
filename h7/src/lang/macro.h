#pragma once


#define H7L_DEF_OBJ_WRAP_BASE(mb)\
void wrapOfPrimitive(sint8 val){\
    mb.initWithWrapPrimitive(kPriType_CHAR, &val);\
}\
void wrapOfPrimitive(uint8 val){\
    mb.initWithWrapPrimitive(kPriType_UCHAR, &val);\
}\
void wrapOfPrimitive(sint16 val){\
    mb.initWithWrapPrimitive(kPriType_SHORT, &val);\
}\
void wrapOfPrimitive(uint16 val){\
    mb.initWithWrapPrimitive(kPriType_USHORT, &val);\
}\
void wrapOfPrimitive(sint32 val){\
    mb.initWithWrapPrimitive(kPriType_INT, &val);\
}\
void wrapOfPrimitive(uint32 val){\
    mb.initWithWrapPrimitive(kPriType_UINT, &val);\
}\
void wrapOfPrimitive(sint64 val){\
    mb.initWithWrapPrimitive(kPriType_LONG, &val);\
}\
void wrapOfPrimitive(uint64 val){\
    mb.initWithWrapPrimitive(kPriType_ULONG, &val);\
}\
void wrapOfPrimitive(float val){\
    mb.initWithWrapPrimitive(kPriType_FLOAT, &val);\
}\
void wrapOfPrimitive(double val){\
    mb.initWithWrapPrimitive(kPriType_DOUBLE, &val);\
}\
void wrapOfPrimitive(bool val){\
    mb.initWithWrapPrimitive(kPriType_BOOL, &val);\
}


#define H7L_DEF_OBJ_WRAP_BASE_PTR(mb)\
void wrapOfPrimitivePtr(sint8* val){\
        mb.initWithWrapPrimitivePtr(kPriType_CHAR, val);\
}\
    void wrapOfPrimitivePtr(uint8* val){\
        mb.initWithWrapPrimitivePtr(kPriType_UCHAR, val);\
}\
    void wrapOfPrimitivePtr(sint16* val){\
        mb.initWithWrapPrimitivePtr(kPriType_SHORT, val);\
}\
    void wrapOfPrimitivePtr(uint16* val){\
        mb.initWithWrapPrimitivePtr(kPriType_USHORT, val);\
}\
    void wrapOfPrimitivePtr(sint32* val){\
        mb.initWithWrapPrimitivePtr(kPriType_INT, val);\
}\
    void wrapOfPrimitivePtr(uint32* val){\
        mb.initWithWrapPrimitivePtr(kPriType_UINT, val);\
}\
    void wrapOfPrimitivePtr(sint64* val){\
        mb.initWithWrapPrimitivePtr(kPriType_LONG, val);\
}\
    void wrapOfPrimitivePtr(uint64* val){\
        mb.initWithWrapPrimitivePtr(kPriType_ULONG, val);\
}\
    void wrapOfPrimitivePtr(float* val){\
        mb.initWithWrapPrimitivePtr(kPriType_FLOAT, val);\
}\
    void wrapOfPrimitivePtr(double* val){\
        mb.initWithWrapPrimitivePtr(kPriType_DOUBLE, val);\
}\
    void wrapOfPrimitivePtr(bool* val){\
        mb.initWithWrapPrimitivePtr(kPriType_BOOL, val);\
}

