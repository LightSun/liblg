#pragma once

namespace h7l {

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
    PRINT,      // 打印寄存器值
    HALT        // 停止虚拟机
};

}
