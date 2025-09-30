#pragma once

#include <stack>
#include "runtime/context.h"
#include "runtime/TypeDelegateFactory.h"
#include "runtime/VMTracker.h"
#include "runtime/Upvalue.h"

namespace h7l { namespace runtime {

// 调用帧
struct CallFrame {
    Closure* closure;
    int pc {0};
    int base; // 寄存器基址
    int numReg;
    //std::vector<Value> registers;

    CallFrame(Closure* cl, int p, int b, int numReg)
        : closure(cl), pc(p), base(b),numReg(numReg) {
        cl->ref();
    }
    ~CallFrame(){
        if(closure){
            closure->unref();
            closure = nullptr;
        }
    }
    CallFrame(CallFrame&& cf){
        this->closure = cf.closure;
        this->pc = cf.pc;
        this->base = cf.base;
        this->numReg = cf.numReg;
        closure->ref();
    }
    CallFrame(const CallFrame& cf){
        this->closure = cf.closure;
        this->pc = cf.pc;
        this->base = cf.base;
        this->numReg = cf.numReg;
        closure->ref();
    }
    CallFrame& operator=(CallFrame&& cf){
        this->closure = cf.closure;
        this->pc = cf.pc;
        this->base = cf.base;
        this->numReg = cf.numReg;
        closure->ref();
        return *this;
    }
    CallFrame& operator=(const CallFrame& cf){
        this->closure = cf.closure;
        this->pc = cf.pc;
        this->base = cf.base;
        this->numReg = cf.numReg;
        closure->ref();
        return *this;
    }
    int getNumRegs()const{
        return numReg;
    }
};

class VM
{
public:
    VM(): pc(0),running(false) {
        globalRegisters_.resize(50);  //初始寄存器数量
    }
    Value& getGlobalRegister(int index) {
        return globalRegisters_[index];
    }
    void setGlobalRegister(int index, const Value& value) {
        if (index >= 0 && index < globalRegisters_.size()) {
            globalRegisters_[index] = value;
        }
    }
    void execute(std::shared_ptr<FunctionProto> func);

private:
    //get current reg
    Value& getRegister(int index) {
        CallFrame& frame = callStack.top();
        if (index >= 0 && index < frame.numReg) {
            //return frame.registers[index];
            return globalRegisters_[frame.base + index];
        }
        MED_ASSERT(false);
    }
    // 关闭所有指向指定栈位置之上的upvalue
    void closeUpvaluesAbove(int stackIndex);
    std::shared_ptr<Upvalue> findOrCreateUpvalue(int stackIndex);

private:
    void trackDiff(const Instruction& ins, CList<int> srcRegs, int dstReg);
    void trackDiff(const Instruction& ins, int srcReg, int dstReg);
    void trackDiff(const Instruction& ins);

private:
    std::vector<Value> globalRegisters_;  // 寄存器数组
    std::stack<CallFrame> callStack;  // 调用栈
    int pc;                        // 程序计数器
    bool running;                  // 运行标志
    bool debug_ {true};
    //
    std::unordered_map<int, std::shared_ptr<Upvalue>> openUpvalues_;
    TypeDelegateFactory m_factory;
    VMTracker m_tracker;
};

}}
