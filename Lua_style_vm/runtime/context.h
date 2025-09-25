#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <atomic>


namespace h7l { namespace runtime {


using String = std::string;
using CString = const std::string&;

#ifdef _WIN32
typedef long long Long;
typedef unsigned long long ULong;
#else
typedef long int Long;
typedef unsigned long int ULong;
#endif

//string
//struct Table
//struct Closure
typedef union TypeValue{
    char s8;
    unsigned char u8;
    short s16;
    unsigned short u16;
    int s32;
    unsigned int u32;
    Long s64;
    ULong u64;
    bool b;
    float f32;
    double f64;
    void* ptr;
}TypeValue;

enum Type{
    kType_NONE = -1,
    kType_S8 ,
    kType_U8,
    kType_S16 ,
    kType_U16 ,
    kType_S32,
    kType_U32 ,
    kType_S64,
    kType_U64,
    kType_Bool,
    kType_FLOAT,
    kType_DOUBLE,
    kType_TABLE,
    kType_CLOSURE,
};


struct IObjectType
{
    std::atomic_int refCnt;

    virtual ~IObjectType(){}

    void ref(){
        refCnt.fetch_add(1, std::memory_order_relaxed);
    }
    void unref(){
        if(refCnt.fetch_add(-1, std::memory_order_relaxed) == 1){
            delete this;
        }
    }
};

struct Value
{
    int type {kType_NONE};
    TypeValue base;

    ~Value(){
        unref();
    }

    IObjectType* getPtr(){
        return (IObjectType*)base.ptr;
    }
    void unref(){
        if(type > kType_DOUBLE){
            getPtr()->unref();
        }
    }

    //------------
    Value(Value&& v){
        this->type = v.type;
        this->base = v.base;
        if(type > kType_DOUBLE){
            getPtr()->ref();
        }
    }
    Value(const Value& v){
        this->type = v.type;
        this->base = v.base;
        if(type > kType_DOUBLE){
            getPtr()->ref();
        }
    }
    Value& operator=(Value&& v){
        unref();
        this->type = v.type;
        this->base = v.base;
        if(type > kType_DOUBLE){
            getPtr()->ref();
        }
        return *this;
    }
    Value& operator=(const Value& v){
        unref();
        this->type = v.type;
        this->base = v.base;
        if(type > kType_DOUBLE){
            getPtr()->ref();
        }
        return *this;
    }
};

struct Table: public IObjectType
{
    std::unordered_map<std::string, Value> fields;
    std::vector<Value> arrayPart;

    Value* get(const std::string& key) {
        if (fields.find(key) != fields.end()) {
            return &fields[key];
        }
        return nullptr;
    }
    Value* get(int index) {
        if (arrayPart.find(index) != arrayPart.end()) {
            return &arrayPart[index];
        }
        return nullptr;
    }

    // 设置值
    void set(const std::string& key, const Value& value) {
        fields[key] = value;
    }

    void set(int index, const std::Value& value) {
        arrayPart[index] = value;
    }

    // 检查是否存在键
    bool contains(const std::string& key) {
        return fields.find(key) != fields.end();
    }
    bool contains(int index) {
        return index >=0 && index < arrayPart.size();
    }
};

struct Instruction {
    int opcode;
    int a;  // 通常用于目标寄存器
    int b;  // 操作数1或常量索引
    int c;  // 操作数2

    Instruction(OpCode op, int a = 0, int b = 0, int c = 0)
        : opcode(op), a(a), b(b), c(c) {}
};

// 函数原型
struct FunctionProto {
    std::vector<Instruction> instructions;
    std::vector<Value> constants;
    std::vector<std::shared_ptr<FunctionProto>> nestedFunctions; // 嵌套函数
    int numParams;
    int numRegisters;
    int upvalueCount; // upvalue数量

    FunctionProto(const std::vector<Instruction>& instrs,
                  const std::vector<Value>& consts = {},
                  const std::vector<std::shared_ptr<FunctionProto>>& nested = {},
                  int params = 0, int regs = 10, int upvalues = 0)
        : instructions(instrs), constants(consts), nestedFunctions(nested),
        numParams(params), numRegisters(regs), upvalueCount(upvalues) {}
};

// 闭包：函数原型 + upvalue数组
class Closure : public IObjectType{
public:
    std::shared_ptr<FunctionProto> proto;
    std::vector<Value> upvalues;

    Closure(std::shared_ptr<FunctionProto> p) : proto(p) {
        upvalues.resize(p->upvalueCount);
    }
};

}}
