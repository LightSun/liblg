#include "VMTracker.h"
#include "runtime/utils/privates.h"

namespace h7l { namespace runtime {


void VMTracker::trackDiff(const Instruction& ins, const Item& item){
    auto str = opcode2Str(ins.opcode);

    std::stringstream ss;
    ss << str << "(format -> [reg]: v)\n";
    for(auto& r : item.srcRegs){
        auto& v = m_saveRegisters[r];
        ss << "\t[Src][" << r << "]: " << v;
    }
    if(item.dstReg >= 0){
        auto& v = m_curRegisters[item.dstReg];
        ss << "\t[Dst][" << item.dstReg << "]: " << v;
    }
    std::cout << ss.str() << std::endl;
}

}}
