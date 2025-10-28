#pragma once

#include <stack>
#include "runtime/CallFrame.h"
#include "runtime/TypeDelegateFactory.h"
#include "runtime/VMTracker.h"
#include "runtime/Upvalue.h"
#include "runtime/Array.h"
#include "runtime/utils/ModuleConstantPool.h"

namespace h7l { namespace runtime {

class VM
{
public:
    VM(std::shared_ptr<ModuleConstantPool> pool):
        const_pool_(pool),pc(0),running(false) {
        globalRegisters_.resize(50);  //初始寄存器数量
    }
    Value& getRegister(CallFrame* frame,int index) {
        if (index >= 0 && index < frame->numReg) {
             return globalRegisters_[frame->base + index];
        }
        MED_ASSERT(false);
    }
    void setRegister(CallFrame* frame,int index, const Value& value) {
        if (index >= 0 && index < frame->numReg) {
            globalRegisters_[frame->base + index] = value;
        }
    }
    void execute(std::shared_ptr<FunctionProto> func);

private:
    void runInst_array(CallFrame& cf, const Instruction& instr);

private:
    //get current reg
    Value& getRegister(int index) {
        CallFrame& frame = callStack_.top();
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
    void processBaseInst(CallFrame& frame, const Instruction& instr);
    void processControlInst(CallFrame& frame, const Instruction& instr);
    void processObjectInst(CallFrame& frame, const Instruction& instr);
    void processOtherInst(CallFrame& frame, const Instruction& instr);

    void trackDiff(const Instruction& ins, CList<int> srcRegs, int dstReg);
    void trackDiff(const Instruction& ins, int srcReg, int dstReg);
    void trackDiff(const Instruction& ins);

private:
    std::shared_ptr<ModuleConstantPool> const_pool_;
    std::vector<Value> globalRegisters_;  // 寄存器数组
    std::stack<CallFrame> callStack_;  // 调用栈
    int pc;                        // 程序计数器
    bool running;                  // 运行标志
    bool debug_ {true};
    //
    std::unordered_map<int, std::shared_ptr<Upvalue>> openUpvalues_;
    TypeDelegateFactory m_factory;
    VMTracker m_tracker;
};

}}
