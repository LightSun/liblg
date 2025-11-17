#pragma once

#include <stack>
#include "runtime/CallFrame.h"
#include "runtime/TypeDelegateFactory.h"
#include "runtime/VMTracker.h"
#include "runtime/Upvalue.h"
#include "runtime/Array.h"
#include "runtime/Exception.h"
#include "runtime/utils/ModuleConstantPool.h"
#include "runtime/utils/VirtualVector.h"

namespace h7l { namespace runtime {


class VM
{
public:
    VM(int coreRegSize = 256):VM(coreRegSize, std::make_shared<ModuleConstantPool>()){
    }
    VM(int coreRegSize,std::shared_ptr<ModuleConstantPool> pool):
        globalRegisters_(coreRegSize),const_pool_(pool),pc(0),running(false) {
    }
    ConstantPoolPlan& getDefautConstantPool(){
        return (*const_pool_)[0];
    }
    Value& getTop(CallFrame* frame){
        return getRegister(frame, 0);
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
    void closeAndPopFrame(CallFrame& frame);
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
    VirtualVector<Value> globalRegisters_;  // 寄存器数组
    std::shared_ptr<ModuleConstantPool> const_pool_;
    std::stack<CallFrame> callStack_;  // 调用栈
    int pc;                        // 程序计数器
    bool running;                  // 运行标志
    bool debug_ {true};
    //
    std::unordered_map<int, std::shared_ptr<Upvalue>> openUpvalues_;
    TypeDelegateFactory m_factory;
    VMTracker m_tracker;
    sk_sp<Exception> m_excep;
};

}}
