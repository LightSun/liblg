#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <memory>
#include <functional>

#include "runtime/Instruction.h"
#include "runtime/Table.h"

namespace h7l { namespace runtime {

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
class Closure : public BaseObjectType<Closure>{
public:
    std::shared_ptr<FunctionProto> proto;
    std::vector<Value> upvalues;

    Closure(std::shared_ptr<FunctionProto> p) : proto(p) {
        upvalues.resize(p->upvalueCount);
    }

    void printTo(std::stringstream& ss)override{
        ss << "Closure@"<< this << "(";
        for(auto& v: upvalues){
            v.printTo(ss);
            ss << "; ";
        }
        ss << ")";
    }
    bool equals(IObjectType* oth) const override{
        return this == oth;
    }
};

class VM;

class CFunction: public BaseObjectType<CFunction>{
public:
    std::function<Value(VM*)> func;
    using CFUNC = Value(*)(VM*);

    CFunction(std::function<Value(VM*)> func):func(func){}
    CFunction(CFUNC func):func(func){}

    void printTo(std::stringstream& ss)override{
        ss << "CFunction@"<< this;
    }
    bool equals(IObjectType* oth) const override{
        return this == oth;
    }
};

}}
