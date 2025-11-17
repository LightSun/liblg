#pragma once

#include <stack>
#include "runtime/context.h"
#include "runtime/LoopState.h"

namespace h7l { namespace runtime {

struct CallFrame {
    Closure* closure;
    std::stack<std::shared_ptr<LoopState>> loopStates;
    int pc {0};// current pc of call-frame
    int base;  // 寄存器基址
    int numReg;

    CallFrame(Closure* cl, int pc, int base, int numReg)
        : closure(cl), pc(pc), base(base),numReg(numReg) {
        cl->ref();
    }
    ~CallFrame(){
        if(closure){
            closure->unref();
            closure = nullptr;
        }
    }
    CallFrame(CallFrame&& cf){
        operator=(cf);
    }
    CallFrame(const CallFrame& cf){
        operator=(cf);
    }
    CallFrame& operator=(CallFrame&& cf){
        this->closure = cf.closure;
        this->pc = cf.pc;
        this->loopStates = cf.loopStates;
        this->base = cf.base;
        this->numReg = cf.numReg;
        closure->ref();
        return *this;
    }
    CallFrame& operator=(const CallFrame& cf){
        this->closure = cf.closure;
        this->pc = cf.pc;
        this->loopStates = cf.loopStates;
        this->base = cf.base;
        this->numReg = cf.numReg;
        closure->ref();
        return *this;
    }
    int getNumRegs()const{
        return numReg;
    }
    int getModuleIndex()const{
        return closure->proto->modIndex;
    }
    Instruction& getInst(int pc){
        return closure->proto->instructions[pc];
    }
};

}}
