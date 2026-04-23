#include "third/src/catch.hpp"
#include "VMTester.h"

using namespace h7l::runtime;
using VM = h7l::runtime::VM;

//Instruction: op, a, b, c
//->GETUPVAL: dst, upindex, -
//->LOADK: dst_reg, src_idx, -
//->SETUPVAL: dst-upval, upindex, -
TEST_CASE( "int test_inst_base()", "[Inst]" ) {
    //instrs, nestedFunctions, numParam, numReg
    std::vector<Instruction> insts = {
        {LOADK, 0, 0, 0},
        {LOADK, 1, 1, 0},
        {ADD, 0, 0, 1},
        {RETURN, 0, 0, 0}
    };
    //0 param.
    auto proto1 = std::make_shared<FunctionProto>(
        insts
    );
    //proto1->addUpvalueDesc(true, 0);
    //
    ConstantPoolPlan pp;
    pp.addLong(1);
    pp.addLong(2);
    pp.execute();
    proto1->consts = pp.getRuntimeValues();
    //
    VMTester tester;
    tester.getVM().execute(proto1);
    auto& val = tester.getRawRegister(0);
    REQUIRE(val.getAsLong() == 3);
}

