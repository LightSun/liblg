#pragma once

#include "runtime/TypeDelegate.h"

#define PRITIVE_TypeDelegate_IMPL(fn)\
Value add(Value& v1, Value& v2)override{\
    return Value(v1.base.fn + v2.base.fn);\
}\
Value sub(Value& v1, Value& v2)override{\
    return Value(v1.base.fn - v2.base.fn);\
}\
Value mul(Value& v1, Value& v2)override{\
    return Value(v1.base.fn * v2.base.fn);\
}\
Value div(Value& v1, Value& v2)override{\
    return Value(v1.base.fn / v2.base.fn);\
}\
Value lessThan(Value& v1, Value& v2)override{\
    return Value(v1.base.fn < v2.base.fn);\
}\
Value lessEquals(Value& v1, Value& v2)override{\
    return Value(v1.base.fn <= v2.base.fn);\
}

#define DEF_TD_DISABLE_FUNC(name)\
Value name(Value& /*v1*/, Value& /*v2*/)override{\
    MED_ASSERT(false);\
    return Value();\
}


namespace h7l { namespace runtime {

class CharTypeDelegate: public ITypeDelegate{
public:
    PRITIVE_TypeDelegate_IMPL(s8);
};

class UCharTypeDelegate: public ITypeDelegate{
public:
    PRITIVE_TypeDelegate_IMPL(u8);
};

class ShortTypeDelegate: public ITypeDelegate{
public:
    PRITIVE_TypeDelegate_IMPL(s16);
};

class UShortTypeDelegate: public ITypeDelegate{
public:
    PRITIVE_TypeDelegate_IMPL(u16);
};

class IntTypeDelegate: public ITypeDelegate{
public:
    PRITIVE_TypeDelegate_IMPL(s32);
};

class UIntTypeDelegate: public ITypeDelegate{
public:
    PRITIVE_TypeDelegate_IMPL(u32);
};

class LongTypeDelegate: public ITypeDelegate{
public:
    PRITIVE_TypeDelegate_IMPL(s64);
};
class ULongTypeDelegate: public ITypeDelegate{
public:
    PRITIVE_TypeDelegate_IMPL(u64);
};

class FloatTypeDelegate: public ITypeDelegate{
public:
    PRITIVE_TypeDelegate_IMPL(f32);
};

class DoubleTypeDelegate: public ITypeDelegate{
public:
    PRITIVE_TypeDelegate_IMPL(f64);
};

}}
