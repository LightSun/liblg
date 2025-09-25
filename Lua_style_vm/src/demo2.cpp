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

namespace demo2{
// 自定义结构体 - 类似于Lua中的表
struct Table {
    std::unordered_map<std::string, std::any> fields;
    std::unordered_map<int, std::any> arrayPart;

    // 获取值
    std::any get(const std::string& key) {
        if (fields.find(key) != fields.end()) {
            return fields[key];
        }
        return nullptr;
    }

    std::any get(int index) {
        if (arrayPart.find(index) != arrayPart.end()) {
            return arrayPart[index];
        }
        return nullptr;
    }

    // 设置值
    void set(const std::string& key, const std::any& value) {
        fields[key] = value;
    }

    void set(int index, const std::any& value) {
        arrayPart[index] = value;
    }

    // 检查是否存在键
    bool contains(const std::string& key) {
        return fields.find(key) != fields.end();
    }

    bool contains(int index) {
        return arrayPart.find(index) != arrayPart.end();
    }
};

// 前向声明
class VM;
class Closure;
struct FunctionProto;

// 值类型：可以存储数字、字符串、布尔值、表、闭包、C函数或nil
using Value = std::variant<double, std::string, bool,
                           std::shared_ptr<Table>,
                          std::shared_ptr<Closure>,
                           std::function<void(VM&)>,
                           std::nullptr_t>;

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
    CLOSURE,    // 创建闭包
    GETUPVAL,   // 获取upvalue
    SETUPVAL,   // 设置upvalue
    CALL_C,     // 调用C函数
    PRINT,      // 打印寄存器值
    HALT        // 停止虚拟机
};

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
    std::vector<std::shared_ptr<FunctionProto>> nestedFunctions; // 嵌套函数
    int numParams;
    int numRegisters;
    int upvalueCount; // upvalue数量

    FunctionProto(const std::vector<Instruction>& instrs,
                 const std::vector<Value>& consts = {},
                  const std::vector<std::shared_ptr<FunctionProto>>& nested = {},
                  int params = 0, int regs = 10, int upvalues = 0)
        : instructions(instrs), constants(consts), nestedFunctions(nested),
        numParams(params), numRegisters(regs), upvalueCount(upvalues) {}
};

// 闭包：函数原型 + upvalue数组
class Closure {
public:
    std::shared_ptr<FunctionProto> proto;
    std::vector<Value> upvalues;

    Closure(std::shared_ptr<FunctionProto> p) : proto(p) {
        upvalues.resize(p->upvalueCount);
    }
};

// 调用帧
struct CallFrame {
    std::shared_ptr<Closure> closure;
    int pc;
    int base; // 寄存器基址

    CallFrame(std::shared_ptr<Closure> cl, int p, int b)
        : closure(cl), pc(p), base(b) {}
};

// 虚拟机状态
class VM {
private:
    std::vector<Value> registers;  // 寄存器数组
    std::stack<CallFrame> callStack;  // 调用栈
    int pc;                        // 程序计数器
    bool running;                  // 运行标志

    // 辅助函数：获取值作为数字
    double getAsNumber(const Value& v) {
        if (std::holds_alternative<double>(v)) {
            return std::get<double>(v);
        }
        std::cerr << "Type error: expected number, got " << valueToString(v) << std::endl;
        return 0;
    }

    // 辅助函数：获取值作为布尔值
    bool getAsBool(const Value& v) {
        if (std::holds_alternative<bool>(v)) {
            return std::get<bool>(v);
        }
        if (std::holds_alternative<double>(v)) {
            return std::get<double>(v) != 0;
        }
        if (std::holds_alternative<std::nullptr_t>(v)) {
            return false;
        }
        if (std::holds_alternative<std::string>(v)) {
            // 空字符串视为false，非空字符串视为true
            return !std::get<std::string>(v).empty();
        }
        return true;  // 其他类型视为真
    }

    // 辅助函数：获取值作为字符串
    std::string getAsString(const Value& v) {
        if (std::holds_alternative<std::string>(v)) {
            return std::get<std::string>(v);
        }
        if (std::holds_alternative<double>(v)) {
            return std::to_string(std::get<double>(v));
        }
        if (std::holds_alternative<bool>(v)) {
            return std::get<bool>(v) ? "true" : "false";
        }
        if (std::holds_alternative<std::nullptr_t>(v)) {
            return "nil";
        }
        return "[complex value]";
    }

    // 辅助函数：获取表引用
    std::shared_ptr<Table> getAsTable(const Value& v) {
        if (std::holds_alternative<std::shared_ptr<Table>>(v)) {
            return std::get<std::shared_ptr<Table>>(v);
        }
        std::cerr << "Type error: expected table, got " << valueToString(v) << std::endl;
        return nullptr;
    }

    // 辅助函数：获取闭包引用
    std::shared_ptr<Closure> getAsClosure(const Value& v) {
        if (std::holds_alternative<std::shared_ptr<Closure>>(v)) {
            return std::get<std::shared_ptr<Closure>>(v);
        }
        std::cerr << "Type error: expected closure, got " << valueToString(v) << std::endl;
        return nullptr;
    }

    // 辅助函数：获取C函数引用
    std::function<void(VM&)> getAsCFunction(const Value& v) {
        if (std::holds_alternative<std::function<void(VM&)>>(v)) {
            return std::get<std::function<void(VM&)>>(v);
        }
        std::cerr << "Type error: expected C function, got " << valueToString(v) << std::endl;
        return nullptr;
    }

    // 辅助函数：值转换为字符串
    std::string valueToString(const Value& v) {
        if (std::holds_alternative<double>(v)) {
            double val = std::get<double>(v);
            // 如果是整数，显示为整数形式
            if (val == static_cast<int>(val)) {
                return std::to_string(static_cast<int>(val));
            }
            return std::to_string(val);
        } else if (std::holds_alternative<std::string>(v)) {
            return std::get<std::string>(v);
        } else if (std::holds_alternative<bool>(v)) {
            return std::get<bool>(v) ? "true" : "false";
        } else if (std::holds_alternative<std::shared_ptr<Table>>(v)) {
            return "[table]";
        } else if (std::holds_alternative<std::shared_ptr<Closure>>(v)) {
            return "[closure]";
        } else if (std::holds_alternative<std::function<void(VM&)>>(v)) {
            return "[C function]";
        } else if (std::holds_alternative<std::nullptr_t>(v)) {
            return "nil";
        } else {
            return "[unknown]";
        }
    }

public:
    VM() : pc(0), running(false) {
        registers.resize(50);  // 初始寄存器数量
    }

    // 执行函数
    void execute(std::shared_ptr<FunctionProto> func) {
        auto closure = std::make_shared<Closure>(func);
        callStack.push(CallFrame(closure, 0, 0));
        running = true;

        // 主执行循环
        while (running && !callStack.empty()) {
            CallFrame& frame = callStack.top();
            auto closure = frame.closure;
            pc = frame.pc;

            if (pc >= closure->proto->instructions.size()) {
                callStack.pop();
                if (!callStack.empty()) {
                    // 返回调用者
                    CallFrame& caller = callStack.top();
                    caller.pc++; // 继续执行下一条指令
                }
                continue;
            }

            const Instruction& instr = closure->proto->instructions[pc];

            switch (instr.opcode) {
            case LOADK: {
                if (instr.b < closure->proto->constants.size()) {
                    registers[frame.base + instr.a] = closure->proto->constants[instr.b];
                } else {
                    std::cerr << "Constant index out of bounds: " << instr.b << std::endl;
                }
                frame.pc++;
                break;
            }

            case LOADBOOL: {
                registers[frame.base + instr.a] = static_cast<bool>(instr.b);
                frame.pc++;
                break;
            }

            case MOVE: {
                registers[frame.base + instr.a] = registers[frame.base + instr.b];
                frame.pc++;
                break;
            }

            case ADD: {
                // 支持数字和字符串的加法
                if (std::holds_alternative<double>(registers[frame.base + instr.b]) &&
                        std::holds_alternative<double>(registers[frame.base + instr.c])) {
                    double val1 = std::get<double>(registers[frame.base + instr.b]);
                    double val2 = std::get<double>(registers[frame.base + instr.c]);
                    registers[frame.base + instr.a] = val1 + val2;
                } else {
                    // 如果有一个操作数不是数字，则进行字符串连接
                    std::string str1 = getAsString(registers[frame.base + instr.b]);
                    std::string str2 = getAsString(registers[frame.base + instr.c]);
                    registers[frame.base + instr.a] = str1 + str2;
                }
                frame.pc++;
                break;
            }

            case CONCAT: {
                // 专门用于字符串连接
                std::string str1 = getAsString(registers[frame.base + instr.b]);
                std::string str2 = getAsString(registers[frame.base + instr.c]);
                registers[frame.base + instr.a] = str1 + str2;
                frame.pc++;
                break;
            }

            case SUB: {
                double val1 = getAsNumber(registers[frame.base + instr.b]);
                double val2 = getAsNumber(registers[frame.base + instr.c]);
                registers[frame.base + instr.a] = val1 - val2;
                frame.pc++;
                break;
            }

            case MUL: {
                double val1 = getAsNumber(registers[frame.base + instr.b]);
                double val2 = getAsNumber(registers[frame.base + instr.c]);
                registers[frame.base + instr.a] = val1 * val2;
                frame.pc++;
                break;
            }

            case DIV: {
                double val1 = getAsNumber(registers[frame.base + instr.b]);
                double val2 = getAsNumber(registers[frame.base + instr.c]);
                if (val2 != 0) {
                    registers[frame.base + instr.a] = val1 / val2;
                } else {
                    std::cerr << "Division by zero!\n";
                    registers[frame.base + instr.a] = 0.0;
                }
                frame.pc++;
                break;
            }

            case CALL: {
                auto func = getAsClosure(registers[frame.base + instr.a]);
                if (func) {
                    // 创建新的调用帧
                    int newBase = frame.base + instr.a + 1; // 参数从当前寄存器之后开始
                    callStack.push(CallFrame(func, 0, newBase));
                } else {
                    std::cerr << "Type error: expected function for call\n";
                    frame.pc++;
                }
                break;
            }

            case CALL_C: {
                auto cfunc = getAsCFunction(registers[frame.base + instr.a]);
                if (cfunc) {
                    cfunc(*this); // 调用C函数
                } else {
                    std::cerr << "Type error: expected C function for call\n";
                }
                frame.pc++;
                break;
            }

            case RETURN: {
                // 返回到调用者
                callStack.pop();
                if (!callStack.empty()) {
                    CallFrame& caller = callStack.top();
                    caller.pc++; // 继续执行下一条指令
                }
                break;
            }

            case JMP: {
                frame.pc = instr.a;
                break;
            }

            case JMPIF: {
                if (getAsBool(registers[frame.base + instr.b])) {
                    frame.pc = instr.a;
                } else {
                    frame.pc++;
                }
                break;
            }

            case JMPNOT: {
                if (!getAsBool(registers[frame.base + instr.b])) {
                    frame.pc = instr.a;
                } else {
                    frame.pc++;
                }
                break;
            }

            case EQ: {
                // 支持多种类型的相等比较
                bool result = false;
                const Value& val1 = registers[frame.base + instr.b];
                const Value& val2 = registers[frame.base + instr.c];

                if (val1.index() == val2.index()) {
                    // 相同类型比较
                    if (std::holds_alternative<double>(val1)) {
                        result = std::get<double>(val1) == std::get<double>(val2);
                    } else if (std::holds_alternative<std::string>(val1)) {
                        result = std::get<std::string>(val1) == std::get<std::string>(val2);
                    } else if (std::holds_alternative<bool>(val1)) {
                        result = std::get<bool>(val1) == std::get<bool>(val2);
                    } else if (std::holds_alternative<std::nullptr_t>(val1)) {
                        result = true; // 两个nil相等
                    } else {
                        // 其他类型比较指针值
                        result = &val1 == &val2;
                    }
                }

                registers[frame.base + instr.a] = result;
                frame.pc++;
                break;
            }

            case LT: {
                // 支持数字和字符串的小于比较
                bool result = false;
                const Value& val1 = registers[frame.base + instr.b];
                const Value& val2 = registers[frame.base + instr.c];

                if (std::holds_alternative<double>(val1) && std::holds_alternative<double>(val2)) {
                    result = std::get<double>(val1) < std::get<double>(val2);
                } else if (std::holds_alternative<std::string>(val1) && std::holds_alternative<std::string>(val2)) {
                    result = std::get<std::string>(val1) < std::get<std::string>(val2);
                } else {
                    std::cerr << "Type error: cannot compare these types with LT\n";
                }

                registers[frame.base + instr.a] = result;
                frame.pc++;
                break;
            }

            case LE: {
                // 支持数字和字符串的小于等于比较
                bool result = false;
                const Value& val1 = registers[frame.base + instr.b];
                const Value& val2 = registers[frame.base + instr.c];

                if (std::holds_alternative<double>(val1) && std::holds_alternative<double>(val2)) {
                    result = std::get<double>(val1) <= std::get<double>(val2);
                } else if (std::holds_alternative<std::string>(val1) && std::holds_alternative<std::string>(val2)) {
                    result = std::get<std::string>(val1) <= std::get<std::string>(val2);
                } else {
                    std::cerr << "Type error: cannot compare these types with LE\n";
                }

                registers[frame.base + instr.a] = result;
                frame.pc++;
                break;
            }

            case NEWTABLE: {
                registers[frame.base + instr.a] = std::make_shared<Table>();
                frame.pc++;
                break;
            }

            case GETTABLE: {
                auto table = getAsTable(registers[frame.base + instr.b]);
                if (table) {
                    // 根据键的类型从表中获取值
                    const Value& key = registers[frame.base + instr.c];
                    if (std::holds_alternative<std::string>(key)) {
                        std::string keyStr = std::get<std::string>(key);
                        if (table->contains(keyStr)) {
                            // 这里简化处理，直接将any转换为Value
                            // 实际实现需要更复杂的类型转换
                            std::any value = table->get(keyStr);
                            try {
                                // 尝试转换为double
                                registers[frame.base + instr.a] = std::any_cast<double>(value);
                            } catch (...) {
                                try {
                                    // 尝试转换为string
                                    registers[frame.base + instr.a] = std::any_cast<std::string>(value);
                                } catch (...) {
                                    try {
                                        // 尝试转换为bool
                                        registers[frame.base + instr.a] = std::any_cast<bool>(value);
                                    } catch (...) {
                                        // 其他类型暂时设置为nil
                                        registers[frame.base + instr.a] = nullptr;
                                    }
                                }
                            }
                        } else {
                            registers[frame.base + instr.a] = nullptr;
                        }
                    } else if (std::holds_alternative<double>(key)) {
                        int index = static_cast<int>(std::get<double>(key));
                        if (table->contains(index)) {
                            std::any value = table->get(index);
                            try {
                                registers[frame.base + instr.a] = std::any_cast<double>(value);
                            } catch (...) {
                                try {
                                    registers[frame.base + instr.a] = std::any_cast<std::string>(value);
                                } catch (...) {
                                    try {
                                        registers[frame.base + instr.a] = std::any_cast<bool>(value);
                                    } catch (...) {
                                        registers[frame.base + instr.a] = nullptr;
                                    }
                                }
                            }
                        } else {
                            registers[frame.base + instr.a] = nullptr;
                        }
                    } else {
                        std::cerr << "Table key must be string or number\n";
                        registers[frame.base + instr.a] = nullptr;
                    }
                } else {
                    registers[frame.base + instr.a] = nullptr;
                }
                frame.pc++;
                break;
            }

            case SETTABLE: {
                auto table = getAsTable(registers[frame.base + instr.a]);
                if (table) {
                    // 根据键的类型设置表中的值
                    const Value& key = registers[frame.base + instr.b];
                    const Value& value = registers[frame.base + instr.c];

                    if (std::holds_alternative<std::string>(key)) {
                        std::string keyStr = std::get<std::string>(key);
                        if (std::holds_alternative<double>(value)) {
                            table->set(keyStr, std::get<double>(value));
                        } else if (std::holds_alternative<std::string>(value)) {
                            table->set(keyStr, std::get<std::string>(value));
                        } else if (std::holds_alternative<bool>(value)) {
                            table->set(keyStr, std::get<bool>(value));
                        } else {
                            table->set(keyStr, nullptr);
                        }
                    } else if (std::holds_alternative<double>(key)) {
                        int index = static_cast<int>(std::get<double>(key));
                        if (std::holds_alternative<double>(value)) {
                            table->set(index, std::get<double>(value));
                        } else if (std::holds_alternative<std::string>(value)) {
                            table->set(index, std::get<std::string>(value));
                        } else if (std::holds_alternative<bool>(value)) {
                            table->set(index, std::get<bool>(value));
                        } else {
                            table->set(index, nullptr);
                        }
                    } else {
                        std::cerr << "Table key must be string or number\n";
                    }
                }
                frame.pc++;
                break;
            }

            case CLOSURE: {
                // 创建闭包
                if (instr.b < closure->proto->nestedFunctions.size()) {
                    auto nestedProto = closure->proto->nestedFunctions[instr.b];
                    auto newClosure = std::make_shared<Closure>(nestedProto);

                    // 设置upvalues (简化处理)
                    // 这里应该根据指令的c字段来设置upvalue的来源
                    // 但为了简化，我们直接从当前函数的寄存器中捕获
                    for (int i = 0; i < nestedProto->upvalueCount && i < instr.c; i++) {
                        newClosure->upvalues[i] = registers[frame.base + i];
                    }

                    registers[frame.base + instr.a] = newClosure;
                } else {
                    std::cerr << "Nested function index out of bounds: " << instr.b << std::endl;
                }
                frame.pc++;
                break;
            }

            case GETUPVAL: {
                // 获取upvalue
                if (instr.b < closure->upvalues.size()) {
                    registers[frame.base + instr.a] = closure->upvalues[instr.b];
                } else {
                    std::cerr << "Upvalue index out of bounds: " << instr.b << std::endl;
                }
                frame.pc++;
                break;
            }

            case SETUPVAL: {
                // 设置upvalue
                if (instr.b < closure->upvalues.size()) {
                    closure->upvalues[instr.b] = registers[frame.base + instr.a];
                } else {
                    std::cerr << "Upvalue index out of bounds: " << instr.b << std::endl;
                }
                frame.pc++;
                break;
            }

            case PRINT: {
                std::cout << valueToString(registers[frame.base + instr.a]) << std::endl;
                frame.pc++;
                break;
            }

            case HALT: {
                running = false;
                break;
            }

            default: {
                std::cerr << "Unknown opcode: " << instr.opcode << std::endl;
                frame.pc++;
                break;
            }
            }
        }
    }

    // 获取寄存器值（用于测试和调试）
    Value getRegister(int index) {
        if (index >= 0 && index < registers.size()) {
            return registers[index];
        }
        return nullptr;
    }

    // 设置寄存器值（用于C函数）
    void setRegister(int index, Value value) {
        if (index >= 0 && index < registers.size()) {
            registers[index] = value;
        }
    }
};

// C库函数示例
void printNumber(VM& vm) {
    Value arg = vm.getRegister(0); // 获取第一个参数
    std::cout << "C function printNumber: " <<
        (std::holds_alternative<double>(arg) ? std::to_string(std::get<double>(arg)) : "NaN")
              << std::endl;
}

void squareRoot(VM& vm) {
    Value arg = vm.getRegister(0);
    if (std::holds_alternative<double>(arg)) {
        double num = std::get<double>(arg);
        if (num >= 0) {
            vm.setRegister(0, sqrt(num)); // 返回值放在第一个寄存器
        } else {
            vm.setRegister(0, nullptr); // 错误情况返回nil
        }
    } else {
        vm.setRegister(0, nullptr); // 错误情况返回nil
    }
}

void addNumbers(VM& vm) {
    Value arg1 = vm.getRegister(0);
    Value arg2 = vm.getRegister(1);
    if (std::holds_alternative<double>(arg1) && std::holds_alternative<double>(arg2)) {
        double result = std::get<double>(arg1) + std::get<double>(arg2);
        vm.setRegister(0, result); // 返回值放在第一个寄存器
    } else {
        vm.setRegister(0, nullptr); // 错误情况返回nil
    }
}

}

// 示例使用
void main_demo2() {
    using namespace demo2;
    // 示例1：嵌套函数和闭包Demo - 修复版本
    std::cout << "=== 嵌套函数和闭包Demo ===" << std::endl;

    // 内部函数：计数器
    auto counterProto = std::make_shared<FunctionProto>(
        std::vector<Instruction>{
            {GETUPVAL, 0, 0, 0},    // 获取upvalue (计数器值)
            {LOADK, 1, 0, 0},       // R1 = 1
            {ADD, 0, 0, 1},         // 计数器加1
            {SETUPVAL, 0, 0, 0},    // 设置upvalue
            {RETURN, 0, 0, 0}       // 返回计数器值
        },
        std::vector<Value>{Value(1.0)}, // 常量1
        std::vector<std::shared_ptr<FunctionProto>>{}, // 无嵌套函数
        0, 2, 1 // 参数数, 寄存器数, upvalue数
        );

    // 外部函数：创建计数器
    auto createCounterProto = std::make_shared<FunctionProto>(
        std::vector<Instruction>{
            {LOADK, 0, 0, 0},       // R0 = 0 (初始计数器值)
            {CLOSURE, 1, 0, 1},     // 创建闭包，使用嵌套函数0，捕获1个upvalue
            {SETUPVAL, 0, 0, 0},    // 设置闭包的upvalue为R0
            {PRINT, 0, 0, 0},       // 打印初始值
            {CALL, 1, 0, 0},        // 调用计数器函数
            {PRINT, 1, 0, 0},       // 打印第一次调用结果
            {CALL, 1, 0, 0},        // 再次调用计数器函数
            {PRINT, 1, 0, 0},       // 打印第二次调用结果
            {RETURN, 0, 0, 0}       // 返回
        },
        std::vector<Value>{Value(0.0)}, // 常量0
        std::vector<std::shared_ptr<FunctionProto>>{counterProto}, // 嵌套函数
        0, 2, 0 // 参数数, 寄存器数, upvalue数
        );

    VM vm1;
    vm1.execute(createCounterProto);

    std::cout << std::endl;

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
            Value(nullptr),         // 3: 占位符，将在运行时替换为C函数
            Value(nullptr),         // 4: 占位符，将在运行时替换为C函数
            Value(20.0),            // 5: 20
            Value(nullptr)          // 6: 占位符，将在运行时替换为C函数
        },
        std::vector<std::shared_ptr<FunctionProto>>{}, // 无嵌套函数
        0, 7, 0 // 参数数, 寄存器数, upvalue数
        );

    // 注册C函数到常量表
    VM vm2;
    // 将C函数放入常量表
    auto cFuncProto = std::const_pointer_cast<FunctionProto>(cFunctionDemo);
    cFuncProto->constants[3] = Value(std::function<void(VM&)>(printNumber));
    cFuncProto->constants[4] = Value(std::function<void(VM&)>(squareRoot));
    cFuncProto->constants[6] = Value(std::function<void(VM&)>(addNumbers));

    vm2.execute(cFunctionDemo);

    std::cout << std::endl;

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
        1, 3, 1 // 1个参数, 3个寄存器, 1个upvalue
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
        1, 3, 1 // 1个参数, 3个寄存器, 1个upvalue
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
            Value(std::string("add")) // 0: "add"
        },
        std::vector<std::shared_ptr<FunctionProto>>{adderProto, multiplierProto},
        2, 6, 0 // 2个参数, 6个寄存器, 0个upvalue
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
            Value(std::string("add")),      // 0: "add"
            Value(5.0),                     // 1: 5
            Value(10.0),                    // 2: 10
            Value(std::string("multiply")), // 3: "multiply"
            Value(3.0),                     // 4: 3
            Value(7.0)                      // 5: 7
        },
        std::vector<std::shared_ptr<FunctionProto>>{factoryProto},
        0, 6, 0 // 参数数, 寄存器数, upvalue数
        );

    VM vm3;
    vm3.execute(useFactoryProto);
}
