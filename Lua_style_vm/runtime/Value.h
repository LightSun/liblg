#pragma once

#include "runtime/IObjectType.h"
#include "runtime/String.h"
#include "runtime/utils/privates.h"

namespace h7l { namespace runtime {

class Closure;
struct Table;
struct CFunction;
struct Array;
struct Exception;

struct Value
{
    Type type {kType_NONE};
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
    Value(Type objType, IObjectType* v, bool ref = false){
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
    static inline Value makeNull(){
        return Value(kType_NULL, nullptr);
    }
    static inline Value makeString(CString str){
        return Value(kType_STRING, new StringRef(str), false);
    }
    static inline Value makeClosure(Closure* p){
        return Value(kType_CLOSURE, (IObjectType*)p, false);
    }
    static inline Value makeTable(Table* p){
        return Value(kType_TABLE, (IObjectType*)p, false);
    }
    static inline Value makeCFunction(CFunction* p){
        return Value(kType_CFUNCTION, (IObjectType*)p, false);
    }
    static inline Value makeArray(Array* p){
        return Value(kType_ARRAY, (IObjectType*)p, false);
    }
    static inline Value makeException(Exception* p){
        return Value(kType_EXCEPTION, (IObjectType*)p, false);
    }
    //
    IObjectType* getPtr0()const{
        return (IObjectType*)base.ptr;
    }
    void unref(){
        if(type > kType_NULL){
            getPtr0()->unref();
            base.ptr = nullptr;
            type = kType_NULL;
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
    bool isBool()const{
        return type == kType_BOOL;
    }
    bool isString()const{
        return type == kType_STRING;
    }
    bool isPointerLike()const{
        return type >= kType_NULL;
    }
    bool isIntLike()const{
        return pri_isIntLike(type);
    }
    bool isFloatLike()const{
        return type == kType_FLOAT || type == kType_DOUBLE;
    }
    bool canCastToNumber()const{
        return isIntLike() || isFloatLike();
    }
    bool getBool()const{
        //MED_ASSERT_X(type == kType_BOOL, "must be bool");
        return base.b;
    }
    bool isArray()const{
        return type == kType_ARRAY;
    }
    bool isTable()const{
        return type == kType_TABLE;
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
        return pri_getAsLong(type, base);
    }
    int getAsInt()const{
        return pri_getAsInt(type, base);
    }
    double getAsNumber()const{
        return pri_getAsNumber(type, base);
    }

public:
    void printTo(std::stringstream& ss){
        val_printTo(type, base, ss);
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
        std::stringstream ss;
        printTo(ss);
        std::cout << ss.str() << std::endl;
    }
    String toString(){
        std::stringstream ss;
        printTo(ss);
        return ss.str();
    }
    friend std::ostream& operator<<(std::ostream& os, const Value& bni){
        auto& b = ((Value&)bni);
        std::stringstream ss;
        b.printTo(ss);
        os << ss.str();
        return os;
    }
    int hashCode()const{
        if(type == kType_NULL || type == kType_VOID){
            return 0;
        }
        if(type > kType_NULL){
            return getPtr0()->hashCode();
        }else{
            return base_hash(type, (void*)&base.s64);
        }
    }
};

inline bool operator==(const Value& a, const Value& b) /*noexcept*/ {
    return a.equals(b);
}

}}
