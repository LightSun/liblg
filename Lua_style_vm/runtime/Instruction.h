#pragma once

namespace h7l { namespace runtime {

//LOAD,MOVE - a,b
//ADD, SUB, MUL, DIV, CONCAT: a,b,c
// 指令操作码
enum OpCode {
    LOADK,      // 将常量加载到寄存器
    LOADBOOL,   // 加载布尔值
    MOVE,       // 在寄存器间移动值
    ADD,        // 加法
    SUB,        // 减法
    MUL,        // 乘法
    DIV,        // 除法
    CONCAT,     // 字符串连接
    CALL,       // 函数调用
    RETURN,     // 从函数返回
    JMP,        // 无条件跳转
    JMPIF,      // 条件跳转（真时跳转）
    JMPNOT,     // 条件跳转（假时跳转）
    EQ,         // 相等比较
    LT,         // 小于比较
    LE,         // 小于等于比较
    NEWTABLE,   // 创建新表
    GETTABLE,   // 从表中获取值
    SETTABLE,   // 设置表中的值
    NEWARRAY,
    GETARRAY,
    SETARRAY,

    CLOSURE,    // 创建闭包
    GETUPVAL,   // 获取upvalue
    SETUPVAL,   // 设置upvalue
    CALL_C,     // 调用C函数
    PRINT,      // 打印寄存器值
    HALT,       // 停止虚拟机

    NEWLABEL,
    JMPLABEL,
    JMPLABEL_POP,
};

struct Instruction {
    OpCode opcode;
    int a;  // 通常用于目标寄存器
    int b;  // 操作数1或常量索引
    int c;  // 操作数2. for closure, this is upvalue count

    Instruction(OpCode op, int a = 0, int b = 0, int c = 0)
        : opcode(op), a(a), b(b), c(c) {}
};

}}
