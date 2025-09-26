#pragma once

#include "runtime/IObjectType.h"
#include "runtime/String.h"
#include "runtime/context.h"
#include "runtime/utils/primitive_cmp.hpp"

namespace h7l { namespace runtime {

struct Value
{
    int type {kType_NONE};
    TypeValue base;

    Value(char v){
        type = kType_S8;
        base.s8 = v;
    }
    Value(unsigned char v){
        type = kType_U8;
        base.u8 = v;
    }
    Value(short v){
        type = kType_S16;
        base.s16 = v;
    }
    Value(unsigned short v){
        type = kType_U16;
        base.u16 = v;
    }
    Value(int v){
        type = kType_S32;
        base.s32 = v;
    }
    Value(unsigned int v){
        type = kType_U32;
        base.u32 = v;
    }
    Value(Long v){
        type = kType_S64;
        base.s64 = v;
    }
    Value(ULong v){
        type = kType_U64;
        base.u64 = v;
    }
    Value(float v){
        type = kType_FLOAT;
        base.f32 = v;
    }
    Value(double v){
        type = kType_DOUBLE;
        base.f64 = v;
    }
    Value(bool v){
        type = kType_BOOL;
        base.b = v;
    }
    Value(int objType, IObjectType* v, bool ref = false){
        type = objType;
        base.ptr = v;
        if(ref && v){
            v->ref();
        }
    }
    Value(){
        type = kType_VOID;
        base.ptr = nullptr;
    }

    ~Value(){
        unref();
    }
    static Value makeNull(){
        return Value(kType_NULL, nullptr);
    }
    static Value makeString(CString str){
        return Value(kType_STRING, new StringRef(str), false);
    }
    //
    IObjectType* getPtr0()const{
        return (IObjectType*)base.ptr;
    }
    void unref(){
        if(type > kType_NULL){
            getPtr0()->unref();
            base.ptr = nullptr;
        }
    }
    //------------
    Value(Value&& v){
        this->type = v.type;
        this->base = v.base;
        if(type > kType_NULL){
            getPtr0()->ref();
        }
    }
    Value(const Value& v){
        this->type = v.type;
        this->base = v.base;
        if(type > kType_NULL){
            getPtr0()->ref();
        }
    }
    Value& operator=(Value&& v){
        unref();
        this->type = v.type;
        this->base = v.base;
        if(type > kType_NULL){
            getPtr0()->ref();
        }
        return *this;
    }
    Value& operator=(const Value& v){
        unref();
        this->type = v.type;
        this->base = v.base;
        if(type > kType_NULL){
            getPtr0()->ref();
        }
        return *this;
    }

    //-----------------------------
public:
    bool isVoid()const{
        return type == kType_VOID;
    }
    bool isNullptr()const{
        return type == kType_NULL;
    }
    bool isString()const{
        return type == kType_STRING;
    }
    bool isIntLike()const{
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
    bool isFloatLike()const{
        return type == kType_FLOAT || type == kType_DOUBLE;
    }
    bool canCastToNumber()const{
        return isIntLike() || isFloatLike();
    }
    bool getBool()const{
        MED_ASSERT_X(type == kType_BOOL, "must be bool");
        return base.b;
    }
    template<typename T>
    T* getPtr()const{
        return (T*)base.ptr;
    }
    template<typename T>
    T* getRefPtr()const{
        if(type > kType_NULL){
            auto ptr = getPtr0();
            ptr->ref();
            return (T*)ptr;
        }
        return nullptr;
    }
    Long getAsLong()const{
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
    double getAsNumber()const{
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

public:
    void printTo(std::ostream& ss){
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

        default:
        {
            getPtr0()->printTo(ss);
        }
        }
    }
    bool equals(const Value& o)const{
        if(isIntLike() && o.isIntLike()){
            return getAsLong() == o.getAsLong();
        }else if(isFloatLike() && o.isFloatLike()){
            if(type == kType_DOUBLE || o.type == kType_DOUBLE){
                auto v1 = getAsNumber();
                auto v2 = getAsNumber();
                return safe_compare(v1, v2);
            }else{
                return base.f32 == o.base.f32;
            }
        }else if(type == o.type){
            if(type > kType_NULL){
                return getPtr0()->equals(o.getPtr0());
            }else{
                return true;
            }
        }else{
            return false;
        }
    }
    void print(){
        std::cout << *this << std::endl;
    }
    friend std::ostream& operator<<(std::ostream& os, const Value& bni){
        ((Value&)bni).printTo(os);
        return os;
    }
};

}}
