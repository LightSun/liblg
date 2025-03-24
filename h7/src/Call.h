#pragma once

namespace h7l {

struct Operation;

struct Call
{
    Operation* pc {nullptr};
    Operation* ret_pc {nullptr};

    void init(Operation* pc, Operation* ret_pc){
        this->pc = pc;
        this->ret_pc = ret_pc;
    }
};

}
