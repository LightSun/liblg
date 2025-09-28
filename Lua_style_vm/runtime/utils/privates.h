#pragma once

#include "runtime/IObjectType.h"
#include "runtime/Instruction.h"
#include "runtime/utils/primitive_cmp.hpp"

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
    case JMP:{return "JMP";}
    case JMPIF:{return "JMPIF";}
    case JMPNOT:{return "JMPNOT";}

    case EQ:{return "EQ";}
    case LE:{return "LE";}
    case LT:{return "LT";}

    case NEWTABLE:{return "NEWTABLE";}
    case SETTABLE:{return "SETTABLE";}
    case CLOSURE:{return "CLOSURE";}
    case GETUPVAL:{return "GETUPVAL";}
    case SETUPVAL:{return "SETUPVAL";}
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
    MED_ASSERT_X(false, "not int-like number.");
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
}

static inline double val_printTo(int type, TypeValue& base, std::stringstream& ss){
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
        ((IObjectType*)base.ptr)->printTo(ss);
    }
    }
}


}}
