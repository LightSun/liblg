#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <stack>
#include <memory>
#include <functional>
#include <variant>
#include <iomanip>
#include <cassert>
#include <unordered_map>
#include <any>
#include <cmath>

#include "runtime/VM.h"
#include "runtime/context.h"

namespace demo21{

using namespace h7l::runtime;

// C库函数示例
Value printNumber(VM* vm,CallFrame* f) {
    Value& arg = vm->getRegister(f, 0); // 获取第一个参数
    std::cout << "C function printNumber: " << arg << std::endl;
    return Value();
}

Value squareRoot(VM* vm,CallFrame* f) {
    Value& arg = vm->getRegister(f, 0);
    if(arg.canCastToNumber()){
        auto num = arg.getAsNumber();
        vm->setRegister(f, 0, sqrt(num));
    }else{
        //wrong
        vm->setRegister(f, 0, Value::makeNull());
    }
    return Value();
}

Value addNumbers(VM* vm, CallFrame* f) {
    Value& arg1 = vm->getRegister(f, 0);
    Value& arg2 = vm->getRegister(f, 1);

    if(arg1.canCastToNumber() && arg2.canCastToNumber()){
        auto num1 = arg1.getAsNumber();
        auto num2 = arg2.getAsNumber();
        vm->setRegister(f, 0, num1 + num2);
    }else{
        //wrong
        vm->setRegister(f, 0, Value::makeNull());
    }
    return Value();
}

//static Value makeCFunc(std::function<Value(VM*)> func){
//    return Value(kType_CFUNCTION, new CFunction(func), false);
//}
typedef Value (*CFUNC)(VM*,CallFrame*);
static Value makeCFunc(CFUNC func){
    return Value(kType_CFUNCTION, new CFunction(func), false);
}

}

static void test1();
static void test2();
static void test3();
extern void main_test_const_pool2();
// 示例使用
void main_demo21() {
    //using namespace h7l::runtime;
    //test1();
    //test2();
    main_test_const_pool2();
}

/*
void test1(){
    using namespace demo21;
    using VM = h7l::runtime::VM;
    // 示例1：嵌套函数和闭包Demo - 修复版本
    std::cout << "=== 嵌套函数和闭包Demo ===" << std::endl;

    // 内部函数：计数器
    auto counterProto = std::make_shared<FunctionProto>(
        std::vector<Instruction>{
            //dst, upindex, -
            {GETUPVAL, 0, 0, 0},    // 获取upvalue (计数器值)
            {LOADK, 1, 0, 0},       // R1 = 1
            {ADD, 0, 0, 1},         // 计数器加1
            {SETUPVAL, 0, 0, 0},    // 设置upvalue
            {RETURN, 0, 0, 0}       // 返回计数器值
        },
        std::vector<Value>{Value(1.0)}, // 常量1
        std::vector<std::shared_ptr<FunctionProto>>{}, //无嵌套函数
        0, 2 //参数数, 寄存器数
        );
    counterProto->addUpvalueDesc(true, 0);

    // 外部函数：创建计数器
    auto createCounterProto = std::make_shared<FunctionProto>(
        std::vector<Instruction>{
            {LOADK, 0, 0, 0},       // R0 = 0 (初始计数器值)
            //dst, nestedIdx, -
            {CLOSURE, 1, 0, 0},     // 创建闭包，使用嵌套函数0
            //reg,-,-
            {PRINT, 0, 0, 0},       // 打印初始值
            //closure, numParam, -
            {CALL, 1, 0, 0},        // 调用计数器函数
            {PRINT, 0, 0, 0},       // 打印第一次调用结果
            {CALL, 1, 0, 0},        // 再次调用计数器函数
            {PRINT, 0, 0, 0},       // 打印第二次调用结果
            {RETURN, 0, 0, 0}       // 返回
        },
        std::vector<Value>{Value(0.0)}, // 常量0
        std::vector<std::shared_ptr<FunctionProto>>{counterProto}, // 嵌套函数
        0, 2 // 参数数, 寄存器数
        );

    VM vm1;
    vm1.execute(createCounterProto);

    std::cout << std::endl;
}

void test2(){
    using namespace demo21;
    using VM = h7l::runtime::VM;
    // 示例2：C函数调用Demo - 修复版本
    std::cout << "=== C函数调用Demo ===" << std::endl;

    auto cFunctionDemo = std::make_shared<FunctionProto>(
        std::vector<Instruction>{
            {LOADK, 0, 0, 0},       // R0 = 25
            {LOADK, 1, 3, 0},       // R1 = printNumber函数
            {CALL_C, 1, 0, 0},      // 调用C函数printNumber

            {LOADK, 0, 1, 0},       // R0 = 16
            {LOADK, 1, 4, 0},       // R1 = squareRoot函数
            {CALL_C, 1, 0, 0},      // 调用C函数squareRoot
            {PRINT, 0, 0, 0},       // 打印平方根结果

            {LOADK, 0, 2, 0},       // R0 = 10
            {LOADK, 1, 5, 0},       // R1 = 20
            {LOADK, 2, 6, 0},       // R2 = addNumbers函数
            {CALL_C, 2, 0, 0},      // 调用C函数addNumbers
            {PRINT, 0, 0, 0},       // 打印加法结果

            {HALT, 0, 0, 0}         // 停止
        },
        std::vector<Value>{
            Value(25.0),            // 0: 25
            Value(16.0),            // 1: 16
            Value(10.0),            // 2: 10
            Value::makeNull(),         // 3: 占位符，将在运行时替换为C函数
            Value::makeNull(),         // 4: 占位符，将在运行时替换为C函数
            Value(20.0),            // 5: 20
            Value::makeNull()          // 6: 占位符，将在运行时替换为C函数
        },
        std::vector<std::shared_ptr<FunctionProto>>{}, // 无嵌套函数
        0, 7 // 参数数, 寄存器数
        );

    // 注册C函数到常量表
    VM vm2;
    // 将C函数放入常量表
    auto cFuncProto = std::const_pointer_cast<FunctionProto>(cFunctionDemo);
//    cFuncProto->constants[3] = makeCFunc(printNumber);
//    cFuncProto->constants[4] = makeCFunc(squareRoot);
//    cFuncProto->constants[6] = makeCFunc(addNumbers);

    vm2.execute(cFunctionDemo);

    std::cout << std::endl;

}

void test3(){
    using namespace demo21;
    using VM = h7l::runtime::VM;
    // 示例3：修复的复杂嵌套函数Demo - 运算器工厂
    std::cout << "=== 修复的复杂嵌套函数Demo: 运算器工厂 ===" << std::endl;

    // 内部函数：加法器
    auto adderProto = std::make_shared<FunctionProto>(
        std::vector<Instruction>{
            {GETUPVAL, 2, 0, 0},    // 获取upvalue (增量值)
            {ADD, 0, 0, 2},         // 参数 + 增量
            {RETURN, 0, 0, 0}       // 返回结果
        },
        std::vector<Value>{},
        std::vector<std::shared_ptr<FunctionProto>>{},
        1, 3// 1个参数, 3个寄存器
        );

    // 内部函数：乘法器
    auto multiplierProto = std::make_shared<FunctionProto>(
        std::vector<Instruction>{
            {GETUPVAL, 2, 0, 0},    // 获取upvalue (乘数)
            {MUL, 0, 0, 2},         // 参数 * 乘数
            {RETURN, 0, 0, 0}       // 返回结果
        },
        std::vector<Value>{},
        std::vector<std::shared_ptr<FunctionProto>>{},
        1, 3// 1个参数, 3个寄存器
        );

    // 工厂函数：创建运算器
    auto factoryProto = std::make_shared<FunctionProto>(
        std::vector<Instruction>{
            {LOADK, 4, 0, 0},       // R4 = "add"
            {EQ, 5, 0, 4},          // 比较操作类型
            {JMPNOT, 7, 5, 0},      // 如果不是"add"，跳转到乘法器创建

            // 创建加法器
            {CLOSURE, 3, 0, 1},     // 创建加法器闭包
            {SETUPVAL, 1, 0, 0},    // 设置加法器的upvalue为操作数
            {MOVE, 0, 3, 0},        // 将加法器移动到R0作为返回值
            {JMP, 5, 0, 0},         // 跳转到结束

            // 创建乘法器
            {CLOSURE, 3, 1, 1},     // 创建乘法器闭包
            {SETUPVAL, 1, 0, 0},    // 设置乘法器的upvalue为操作数
            {MOVE, 0, 3, 0},        // 将乘法器移动到R0作为返回值

            // 结束
            {RETURN, 0, 0, 0}       // 返回创建的运算器
        },
        std::vector<Value>{
            Value::makeString("add") // 0: "add"
        },
        std::vector<std::shared_ptr<FunctionProto>>{adderProto, multiplierProto},
        2, 6// 2个参数, 6个寄存器
        );

    // 使用工厂函数
    auto useFactoryProto = std::make_shared<FunctionProto>(
        std::vector<Instruction>{
            // 创建加法器 (加5)
            {LOADK, 0, 0, 0},       // R0 = "add"
            {LOADK, 1, 1, 0},       // R1 = 5
            {CLOSURE, 2, 0, 0},     // 创建工厂函数闭包
            {CALL, 2, 2, 0},        // 调用工厂函数

            // 测试加法器
            {MOVE, 3, 0, 0},        // R3 = 加法器函数
            {LOADK, 4, 2, 0},       // R4 = 10
            {CALL, 3, 1, 0},        // 调用加法器
            {PRINT, 3, 0, 0},       // 打印结果 (10 + 5 = 15)

            // 创建乘法器 (乘3)
            {LOADK, 0, 3, 0},       // R0 = "multiply"
            {LOADK, 1, 4, 0},       // R1 = 3
            {CALL, 2, 2, 0},        // 调用工厂函数

            // 测试乘法器
            {MOVE, 3, 0, 0},        // R3 = 乘法器函数
            {LOADK, 4, 5, 0},       // R4 = 7
            {CALL, 3, 1, 0},        // 调用乘法器
            {PRINT, 3, 0, 0},       // 打印结果 (7 * 3 = 21)

            {HALT, 0, 0, 0}         // 停止
        },
        std::vector<Value>{
            Value::makeString("add"),      // 0: "add"
            Value(5.0),                     // 1: 5
            Value(10.0),                    // 2: 10
            Value::makeString("multiply"), // 3: "multiply"
            Value(3.0),                     // 4: 3
            Value(7.0)                      // 5: 7
        },
        std::vector<std::shared_ptr<FunctionProto>>{factoryProto},
        0, 6// 参数数, 寄存器数
        );

    VM vm3;
    vm3.execute(useFactoryProto);
}
*/
