#pragma once

namespace h7l { namespace runtime {

//LOAD,MOVE - a,b
//ADD, SUB, MUL, DIV, CONCAT: a,b,c
// 指令操作码
enum OpCode {
    BASE_START,
    LOADK,      // 将常量加载到寄存器
//    LOADIMM_S8, // load direct number/bool/string
//    LOADIMM_U8,
//    LOADIMM_S16,
//    LOADIMM_U16,
//    LOADIMM_S32,
//    LOADIMM_U32,
//    LOADIMM_S64,
//    LOADIMM_U64,
    MOVE,       // 在寄存器间移动值
    //------
    ADD,        // 加法
    SUB,        // 减法
    MUL,        // 乘法
    DIV,        // 除法
    //CONCAT,     // 字符串连接
    EQ,         // 相等比较
    LT,         // 小于比较
    LE,         // 小于等于比较

    BASE_END,

    CONTROL_START,
    CALL,       // 函数调用
    CALL_C,     // 调用C函数
    RETURN,     // 从函数返回

    JMPNOT,
    JMP,
    JMPIF,
    NEWLABEL,
    JMPLABEL,
    JMPLABEL_IF,    // 条件跳转（真时跳转）
    JMPLABEL_NOT,   // 条件跳转（假时跳转）

    FORI_INIT,
    FORP_INIT,
    FORP_NEXT,
    LOOP_BREAK,
    LOOP_CONTINUE,
    LOOP_END,      //mark inst for loop

    CONTROL_END,

    OBJECT_START,
    NEWTABLE,   // 创建新表
    GETTABLE,   // 从表中获取值
    SETTABLE,   // 设置表中的值

    NEWARRAY,
    GETARRAY,
    SETARRAY,

    CLOSURE,    // 创建闭包
    GETUPVAL,   // 获取upvalue
    SETUPVAL,   // 设置upvalue
    OBJECT_END,

    PRINT,      // 打印寄存器值
    HALT,       // 停止虚拟机

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
