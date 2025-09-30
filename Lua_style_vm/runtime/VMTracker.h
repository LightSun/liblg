#pragma once

#include "runtime/Value.h"
#include "runtime/Instruction.h"

namespace h7l { namespace runtime {

class VMTracker{
public:
    struct Item{
        std::vector<int> srcRegs;
        int dstReg {-1};

        Item(){
        }
        Item(int src, int dst){
            this->srcRegs.push_back(src);
            this->dstReg = dst;
        }
        Item(const std::vector<int>& src, int dst){
            this->srcRegs = src;
            this->dstReg = dst;
        }
    };
    void saveRegisters(std::vector<Value>& vals, int start, int cnt){
        m_saveRegisters = {vals.begin() + start, vals.begin() + start + cnt};
        m_curRegisters = &vals[start];
    }

    void saveRegisters(std::vector<Value>& vals){
        m_saveRegisters = vals;
        m_curRegisters = vals.data();
    };
    void clearRegisters(){
        m_saveRegisters.clear();
        m_curRegisters = nullptr;
    }
    void trackDiff(const Instruction&, const Item&);

private:
    std::vector<Value> m_saveRegisters;
    Value* m_curRegisters {nullptr};//arr
};

}}
