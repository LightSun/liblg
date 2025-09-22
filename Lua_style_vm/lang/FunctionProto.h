#pragma once

#include <vector>
#include "OpCode.h"
#include "Value.h"

namespace h7l {

// 指令结构
struct Instruction {
    OpCode opcode;
    int a;  // 通常用于目标寄存器
    int b;  // 操作数1或常量索引
    int c;  // 操作数2

    Instruction(OpCode op, int a = 0, int b = 0, int c = 0)
        : opcode(op), a(a), b(b), c(c) {}
};

// 函数原型
struct FunctionProto {
    std::vector<Instruction> instructions;
    std::vector<Value> constants;
    int numParams;
    int numRegisters;

    FunctionProto(const std::vector<Instruction>& instrs,
                 const std::vector<Value>& consts = {},
                  int params = 0, int regs = 10)
        : instructions(instrs), constants(consts),
          numParams(params), numRegisters(regs) {}
};

}
