#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <memory>

#include "runtime/Instruction.h"
#include "runtime/Value.h"
#include "runtime/String.h"

namespace h7l { namespace runtime {

struct Table: public BaseObjectType<Table>
{
    std::unordered_map<std::string, Value> fields;
    std::vector<Value> arrayPart;

    Value* get(const std::string& key) {
        if (fields.find(key) != fields.end()) {
            return &fields[key];
        }
        return nullptr;
    }
    Value* get(size_t index) {
        if (index < arrayPart.size()) {
            return &arrayPart[index];
        }
        return nullptr;
    }
    void set(const std::string& key, const Value& value) {
        fields[key] = value;
    }
    void set(size_t index, const Value& value) {
        arrayPart[index] = value;
    }
    bool contains(const std::string& key) {
        return fields.find(key) != fields.end();
    }
    bool contains(size_t index) {
        return index >=0 && index < arrayPart.size();
    }
//---------------
    void printTo(std::ostream& ss)override{
        if(arrayPart.empty()){
            ss << "{";
            for(auto& [k,v] : fields){
                ss << k << ": " << v;
            }
            ss << "}";
        }else{
            ss << "[";
            int size = arrayPart.size();
            for(int i = 0 ; i < size; ++i){
                ss << arrayPart[i];
                if(i != size - 1){
                    ss << ", ";
                }
            }
            ss << "]";
        }
    }
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
class Closure : public BaseObjectType<Closure>{
public:
    std::shared_ptr<FunctionProto> proto;
    std::vector<Value> upvalues;

    Closure(std::shared_ptr<FunctionProto> p) : proto(p) {
        upvalues.resize(p->upvalueCount);
    }

    void printTo(std::ostream& ss)override{
         ss << "Closure@"<< this;
    }
};

class VM;

class CFunction: public BaseObjectType<CFunction>{
public:
    std::function<Value(VM*)> func;

    CFunction(std::function<Value(VM*)> func):func(func){}

    void printTo(std::ostream& ss)override{
        ss << "CFunction@"<< this;
    }
};

}}
