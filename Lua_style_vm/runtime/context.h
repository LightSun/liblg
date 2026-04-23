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
#include "runtime/Upvalue.h"

namespace h7l { namespace runtime {

class VM;
struct CallFrame;

// 函数原型
struct FunctionProto{
    std::vector<Instruction> instructions;
    std::vector<std::shared_ptr<FunctionProto>> nestedFunctions; //嵌套函数
    std::vector<Value*> consts;//const or static var
    int numParams;
    int numRegs; //include param
    std::vector<std::pair<bool, int>> upvDescs; //upvalue描述 (是否是父函数的局部变量, 索引)

    FunctionProto(const std::vector<Instruction>& instrs,
                  const std::vector<std::shared_ptr<FunctionProto>>& nested = {},
                  int params = 0, int regs = 10)
        : instructions(instrs), nestedFunctions(nested),
        numParams(params), numRegs(regs){}

    void addUpvalueDesc(bool isParentlocal, int index){
        upvDescs.push_back({isParentlocal, index});
    }
    void addConst(Value* ptr){
        consts.push_back(ptr);
    }
};

// 闭包：函数原型 + upvalue数组
class Closure: public BaseObjectType<Closure>{
public:
    std::shared_ptr<FunctionProto> proto;
    std::vector<std::shared_ptr<Upvalue>> upvalues;
    std::vector<int> labelPcs; //pc: 程序计数器

    Closure(std::shared_ptr<FunctionProto> p) : proto(p) {
        upvalues.resize(p->upvDescs.size());
    }
    ~Closure(){
       // fprintf(stderr, "~Closure\n");
    }

    void printTo(std::stringstream& ss)override{
        ss << "Closure@"<< this << "(";
        for(auto& v: upvalues){
            v->get()->printTo(ss);
            ss << "; ";
        }
        ss << ")";
    }
    bool equals(IObjectType* oth) const override{
        return this == oth;
    }
    int hashCode()const override{
        void* ptr = (void*)this;
        return base_hash(kType_S64, ptr);
    }
};

class CFunction: public BaseObjectType<CFunction>{
public:
    using CFUNC = Value(*)(VM*,CallFrame*);
    CFUNC func;

    CFunction(CFUNC func):func(func){}

    void printTo(std::stringstream& ss)override{
        ss << "CFunction@"<< (void*)func;
    }
    bool equals(IObjectType* oth) const override{
        if(this == oth){
            return true;
        }
        auto cf = dynamic_cast<CFunction*>(oth);
        if(cf && cf->func == func){
            return true;
        }
        return false;
    }
    int hashCode()const override{
        Long val = (Long)func;
        return base_hash(kType_S64, &val);
    }
};

}}
