#pragma once

#include <stack>
#include "runtime/context.h"
#include "runtime/TypeDelegateFactory.h"
#include "runtime/VMTracker.h"

namespace h7l { namespace runtime {

// 调用帧
struct CallFrame {
    Closure* closure;
    int pc;
    int base; // 寄存器基址

    CallFrame(Closure* cl, int p, int b)
        : closure(cl), pc(p), base(b) {
        cl->ref();
    }

    ~CallFrame(){
        if(closure){
            closure->unref();
            closure = nullptr;
        }
    }
};

class VM
{
public:
    VM(): pc(0),running(false) {
        registers.resize(50);  //初始寄存器数量
    }
    Value& getRegister(int index) {
        if (index >= 0 && index < registers.size()) {
            return registers[index];
        }
        MED_ASSERT(false);
    }
    void setRegister(int index, const Value& value) {
        if (index >= 0 && index < registers.size()) {
            registers[index] = value;
        }
    }
    void execute(std::shared_ptr<FunctionProto> func);

private:
    void trackDiff(const Instruction& ins, CList<int> srcRegs, int dstReg);
    void trackDiff(const Instruction& ins, int srcReg, int dstReg);
    void trackDiff(const Instruction& ins);

private:
    std::vector<Value> registers;  // 寄存器数组
    std::stack<CallFrame> callStack;  // 调用栈
    int pc;                        // 程序计数器
    bool running;                  // 运行标志
    bool debug_ {true};
    TypeDelegateFactory m_factory;
    VMTracker m_tracker;
};

}}
