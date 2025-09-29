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
#include <unordered_set>

namespace demo2 {

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
using Value = std::variant<double, std::string, bool, std::shared_ptr<Table>,
                          std::shared_ptr<Closure>, std::function<void(VM&)>, std::nullptr_t>;

// Upvalue引用 - 改进的实现
struct Upvalue {
    Value* value;           // 指向值的指针（可能是栈位置或堆位置）
    Value closedValue;      // 当upvalue关闭时存储的值
    bool isClosed;          // 是否已关闭（不再指向栈）

    Upvalue(Value* v) : value(v), isClosed(false) {}

    // 获取值
    Value get() {
        if (isClosed) {
            return closedValue;
        }
        return *value;
    }

    // 设置值
    void set(const Value& v) {
        if (isClosed) {
            closedValue = v;
        } else {
            *value = v;
        }
    }

    // 关闭upvalue（当函数返回时）
    void close() {
        if (!isClosed) {
            closedValue = *value;
            isClosed = true;
            value = nullptr;
        }
    }
};

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
    std::vector<std::pair<bool, int>> upvalueDescriptions; // upvalue描述 (是否是父函数的局部变量, 索引)

    FunctionProto(const std::vector<Instruction>& instrs,
                 const std::vector<Value>& consts = {},
                  const std::vector<std::shared_ptr<FunctionProto>>& nested = {},
                  int params = 0, int regs = 10,
                  const std::vector<std::pair<bool, int>>& upvalues = {})
        : instructions(instrs), constants(consts), nestedFunctions(nested),
        numParams(params), numRegisters(regs), upvalueDescriptions(upvalues) {}
};

// 闭包：函数原型 + upvalue数组
class Closure {
public:
    std::shared_ptr<FunctionProto> proto;
    std::vector<std::shared_ptr<Upvalue>> upvalues;

    Closure(std::shared_ptr<FunctionProto> p) : proto(p) {
        upvalues.resize(p->upvalueDescriptions.size());
    }
};

// 调用帧
struct CallFrame {
    std::shared_ptr<Closure> closure;
    int pc;
    int base; // 寄存器基址
    std::vector<Value> registers; // 该帧的寄存器

    CallFrame(std::shared_ptr<Closure> cl, int p, int b, int numRegisters)
        : closure(cl), pc(p), base(b) {
        registers.resize(numRegisters);
    }
};

// 虚拟机状态
class VM {
private:
    std::vector<Value> globalRegisters;  // 全局寄存器数组（用于跨帧共享）
    std::stack<CallFrame> callStack;  // 调用栈
    bool running;                  // 运行标志

    // 已创建的upvalue缓存（用于共享upvalue）
    std::unordered_map<int, std::shared_ptr<Upvalue>> openUpvalues;

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

    // 获取当前调用帧
    CallFrame& currentFrame() {
        return callStack.top();
    }

    // 获取寄存器值（相对于当前帧）
    Value& getRegister(int index) {
        CallFrame& frame = currentFrame();
        if (index >= 0 && index < frame.registers.size()) {
            return frame.registers[index];
        }
        static Value nil = nullptr;
        return nil;
    }

    // 查找或创建upvalue
    std::shared_ptr<Upvalue> findOrCreateUpvalue(int stackIndex) {
        // 检查是否已经有这个upvalue
        if (openUpvalues.find(stackIndex) != openUpvalues.end()) {
            return openUpvalues[stackIndex];
        }

        // 创建新的upvalue
        auto upvalue = std::make_shared<Upvalue>(&globalRegisters[stackIndex]);
        openUpvalues[stackIndex] = upvalue;
        return upvalue;
    }

    // 关闭所有指向指定栈位置之上的upvalue
    void closeUpvaluesAbove(int stackIndex) {
        std::vector<int> toRemove;
        for (auto& pair : openUpvalues) {
            if (pair.first >= stackIndex) {
                pair.second->close();
                toRemove.push_back(pair.first);
            }
        }
        for (int index : toRemove) {
            openUpvalues.erase(index);
        }
    }

public:
    VM() : running(false) {
        globalRegisters.resize(100);  // 全局寄存器数量
    }

    // 执行函数
    void execute(std::shared_ptr<FunctionProto> func) {
        auto closure = std::make_shared<Closure>(func);
        callStack.push(CallFrame(closure, 0, 0, func->numRegisters));
        running = true;

        // 主执行循环
        while (running && !callStack.empty()) {
            CallFrame& frame = currentFrame();
            auto closure = frame.closure;

            if (frame.pc >= closure->proto->instructions.size()) {
                // 函数结束，关闭upvalues
                closeUpvaluesAbove(frame.base);
                callStack.pop();
                if (!callStack.empty()) {
                    // 返回调用者
                    CallFrame& caller = currentFrame();
                    caller.pc++; // 继续执行下一条指令
                }
                continue;
            }

            const Instruction& instr = closure->proto->instructions[frame.pc];

            switch (instr.opcode) {
            case LOADK: {
                if (instr.b < closure->proto->constants.size()) {
                    getRegister(instr.a) = closure->proto->constants[instr.b];
                } else {
                    std::cerr << "Constant index out of bounds: " << instr.b << std::endl;
                }
                frame.pc++;
                break;
            }

            case LOADBOOL: {
                getRegister(instr.a) = static_cast<bool>(instr.b);
                frame.pc++;
                break;
            }

            case MOVE: {
                getRegister(instr.a) = getRegister(instr.b);
                frame.pc++;
                break;
            }

            case ADD: {
                // 支持数字和字符串的加法
                if (std::holds_alternative<double>(getRegister(instr.b)) &&
                        std::holds_alternative<double>(getRegister(instr.c))) {
                    double val1 = std::get<double>(getRegister(instr.b));
                    double val2 = std::get<double>(getRegister(instr.c));
                    getRegister(instr.a) = val1 + val2;
                } else {
                    // 如果有一个操作数不是数字，则进行字符串连接
                    std::string str1 = getAsString(getRegister(instr.b));
                    std::string str2 = getAsString(getRegister(instr.c));
                    getRegister(instr.a) = str1 + str2;
                }
                frame.pc++;
                break;
            }

            case CONCAT: {
                // 专门用于字符串连接
                std::string str1 = getAsString(getRegister(instr.b));
                std::string str2 = getAsString(getRegister(instr.c));
                getRegister(instr.a) = str1 + str2;
                frame.pc++;
                break;
            }

            case SUB: {
                double val1 = getAsNumber(getRegister(instr.b));
                double val2 = getAsNumber(getRegister(instr.c));
                getRegister(instr.a) = val1 - val2;
                frame.pc++;
                break;
            }

            case MUL: {
                double val1 = getAsNumber(getRegister(instr.b));
                double val2 = getAsNumber(getRegister(instr.c));
                getRegister(instr.a) = val1 * val2;
                frame.pc++;
                break;
            }

            case DIV: {
                double val1 = getAsNumber(getRegister(instr.b));
                double val2 = getAsNumber(getRegister(instr.c));
                if (val2 != 0) {
                    getRegister(instr.a) = val1 / val2;
                } else {
                    std::cerr << "Division by zero!\n";
                    getRegister(instr.a) = 0.0;
                }
                frame.pc++;
                break;
            }

            case CALL: {
                auto func = getAsClosure(getRegister(instr.a));
                if (func) {
                    // 创建新的调用帧
                    int newBase = frame.base + frame.registers.size(); // 新的基址
                    callStack.push(CallFrame(func, 0, newBase, func->proto->numRegisters));

                    // 复制参数
                    for (int i = 0; i < func->proto->numParams && i < instr.b; i++) {
                        globalRegisters[newBase + i] = getRegister(instr.a + 1 + i);
                    }
                } else {
                    std::cerr << "Type error: expected function for call\n";
                    frame.pc++;
                }
                break;
            }

            case CALL_C: {
                auto cfunc = getAsCFunction(getRegister(instr.a));
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
                closeUpvaluesAbove(frame.base);
                callStack.pop();
                if (!callStack.empty()) {
                    CallFrame& caller = currentFrame();
                    caller.pc++; // 继续执行下一条指令
                }
                break;
            }

            case JMP: {
                frame.pc = instr.a;
                break;
            }

            case JMPIF: {
                if (getAsBool(getRegister(instr.b))) {
                    frame.pc = instr.a;
                } else {
                    frame.pc++;
                }
                break;
            }

            case JMPNOT: {
                if (!getAsBool(getRegister(instr.b))) {
                    frame.pc = instr.a;
                } else {
                    frame.pc++;
                }
                break;
            }

            case EQ: {
                // 支持多种类型的相等比较
                bool result = false;
                const Value& val1 = getRegister(instr.b);
                const Value& val2 = getRegister(instr.c);

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

                getRegister(instr.a) = result;
                frame.pc++;
                break;
            }

            case LT: {
                // 支持数字和字符串的小于比较
                bool result = false;
                const Value& val1 = getRegister(instr.b);
                const Value& val2 = getRegister(instr.c);

                if (std::holds_alternative<double>(val1) && std::holds_alternative<double>(val2)) {
                    result = std::get<double>(val1) < std::get<double>(val2);
                } else if (std::holds_alternative<std::string>(val1) && std::holds_alternative<std::string>(val2)) {
                    result = std::get<std::string>(val1) < std::get<std::string>(val2);
                } else {
                    std::cerr << "Type error: cannot compare these types with LT\n";
                }

                getRegister(instr.a) = result;
                frame.pc++;
                break;
            }

            case LE: {
                // 支持数字和字符串的小于等于比较
                bool result = false;
                const Value& val1 = getRegister(instr.b);
                const Value& val2 = getRegister(instr.c);

                if (std::holds_alternative<double>(val1) && std::holds_alternative<double>(val2)) {
                    result = std::get<double>(val1) <= std::get<double>(val2);
                } else if (std::holds_alternative<std::string>(val1) && std::holds_alternative<std::string>(val2)) {
                    result = std::get<std::string>(val1) <= std::get<std::string>(val2);
                } else {
                    std::cerr << "Type error: cannot compare these types with LE\n";
                }

                getRegister(instr.a) = result;
                frame.pc++;
                break;
            }

            case NEWTABLE: {
                getRegister(instr.a) = std::make_shared<Table>();
                frame.pc++;
                break;
            }

            case GETTABLE: {
                auto table = getAsTable(getRegister(instr.b));
                if (table) {
                    // 根据键的类型从表中获取值
                    const Value& key = getRegister(instr.c);
                    if (std::holds_alternative<std::string>(key)) {
                        std::string keyStr = std::get<std::string>(key);
                        if (table->contains(keyStr)) {
                            // 这里简化处理，直接将any转换为Value
                            // 实际实现需要更复杂的类型转换
                            std::any value = table->get(keyStr);
                            try {
                                // 尝试转换为double
                                getRegister(instr.a) = std::any_cast<double>(value);
                            } catch (...) {
                                try {
                                    // 尝试转换为string
                                    getRegister(instr.a) = std::any_cast<std::string>(value);
                                } catch (...) {
                                    try {
                                        // 尝试转换为bool
                                        getRegister(instr.a) = std::any_cast<bool>(value);
                                    } catch (...) {
                                        // 其他类型暂时设置为nil
                                        getRegister(instr.a) = nullptr;
                                    }
                                }
                            }
                        } else {
                            getRegister(instr.a) = nullptr;
                        }
                    } else if (std::holds_alternative<double>(key)) {
                        int index = static_cast<int>(std::get<double>(key));
                        if (table->contains(index)) {
                            std::any value = table->get(index);
                            try {
                                getRegister(instr.a) = std::any_cast<double>(value);
                            } catch (...) {
                                try {
                                    getRegister(instr.a) = std::any_cast<std::string>(value);
                                } catch (...) {
                                    try {
                                        getRegister(instr.a) = std::any_cast<bool>(value);
                                    } catch (...) {
                                        getRegister(instr.a) = nullptr;
                                    }
                                }
                            }
                        } else {
                            getRegister(instr.a) = nullptr;
                        }
                    } else {
                        std::cerr << "Table key must be string or number\n";
                        getRegister(instr.a) = nullptr;
                    }
                } else {
                    getRegister(instr.a) = nullptr;
                }
                frame.pc++;
                break;
            }

            case SETTABLE: {
                auto table = getAsTable(getRegister(instr.a));
                if (table) {
                    // 根据键的类型设置表中的值
                    const Value& key = getRegister(instr.b);
                    const Value& value = getRegister(instr.c);

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

                    // 设置upvalues
                    for (int i = 0; i < nestedProto->upvalueDescriptions.size(); i++) {
                        const auto& desc = nestedProto->upvalueDescriptions[i];
                        if (desc.first) {
                            // 父函数的局部变量
                            newClosure->upvalues[i] = findOrCreateUpvalue(frame.base + desc.second);
                        } else {
                            // 父函数的upvalue
                            newClosure->upvalues[i] = closure->upvalues[desc.second];
                        }
                    }

                    getRegister(instr.a) = newClosure;
                } else {
                    std::cerr << "Nested function index out of bounds: " << instr.b << std::endl;
                }
                frame.pc++;
                break;
            }

            case GETUPVAL: {
                // 获取upvalue
                if (instr.b < closure->upvalues.size()) {
                    getRegister(instr.a) = closure->upvalues[instr.b]->get();
                } else {
                    std::cerr << "Upvalue index out of bounds: " << instr.b << std::endl;
                }
                frame.pc++;
                break;
            }

            case SETUPVAL: {
                // 设置upvalue
                if (instr.b < closure->upvalues.size()) {
                    closure->upvalues[instr.b]->set(getRegister(instr.a));
                } else {
                    std::cerr << "Upvalue index out of bounds: " << instr.b << std::endl;
                }
                frame.pc++;
                break;
            }

            case PRINT: {
                std::cout << valueToString(getRegister(instr.a)) << std::endl;
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

    // 获取全局寄存器值（用于测试和调试）
    Value getGlobalRegister(int index) {
        if (index >= 0 && index < globalRegisters.size()) {
            return globalRegisters[index];
        }
        return nullptr;
    }

    // 设置全局寄存器值（用于C函数）
    void setGlobalRegister(int index, Value value) {
        if (index >= 0 && index < globalRegisters.size()) {
            globalRegisters[index] = value;
        }
    }
};

// C库函数示例
void printNumber(VM& vm) {
    Value arg = vm.getGlobalRegister(0); // 获取第一个参数
    std::cout << "C function printNumber: " <<
        (std::holds_alternative<double>(arg) ? std::to_string(std::get<double>(arg)) : "NaN")
              << std::endl;
}

void squareRoot(VM& vm) {
    Value arg = vm.getGlobalRegister(0);
    if (std::holds_alternative<double>(arg)) {
        double num = std::get<double>(arg);
        if (num >= 0) {
            vm.setGlobalRegister(0, sqrt(num)); // 返回值放在第一个寄存器
        } else {
            vm.setGlobalRegister(0, nullptr); // 错误情况返回nil
        }
    } else {
        vm.setGlobalRegister(0, nullptr); // 错误情况返回nil
    }
}

void addNumbers(VM& vm) {
    Value arg1 = vm.getGlobalRegister(0);
    Value arg2 = vm.getGlobalRegister(1);
    if (std::holds_alternative<double>(arg1) && std::holds_alternative<double>(arg2)) {
        double result = std::get<double>(arg1) + std::get<double>(arg2);
        vm.setGlobalRegister(0, result); // 返回值放在第一个寄存器
    } else {
        vm.setGlobalRegister(0, nullptr); // 错误情况返回nil
    }
}

}
// 示例使用
void main_demo2() {
//    using namespace demo2;
//    // 示例1：改进的嵌套函数和闭包Demo
//    std::cout << "=== 改进的嵌套函数和闭包Demo ===" << std::endl;

//    // 内部函数：计数器
//    auto counterProto = std::make_shared<FunctionProto>(
//        std::vector<Instruction>{
//            {GETUPVAL, 0, 0, 0},    // 获取upvalue (计数器值)
//            {LOADK, 1, 0, 0},       // R1 = 1
//            {ADD, 0, 0, 1},         // 计数器加1
//            {SETUPVAL, 0, 0, 0},    // 设置upvalue
//            {RETURN, 0, 0, 0}       // 返回计数器值
//        },
//        std::vector<Value>{Value(1.0)}, // 常量1
//        std::vector<std::shared_ptr<FunctionProto>>{}, // 无嵌套函数
//        0, 2, // 参数数, 寄存器数
//        {{true, 0}} // upvalue描述: 父函数的第0个局部变量
//        );

//    // 外部函数：创建计数器
//    auto createCounterProto = std::make_shared<FunctionProto>(
//        std::vector<Instruction>{
//            {LOADK, 0, 0, 0},       // R0 = 0 (初始计数器值)
//            {CLOSURE, 1, 0, 0},     // 创建闭包，使用嵌套函数0
//            {PRINT, 0, 0, 0},       // 打印初始值
//            {CALL, 1, 0, 0},        // 调用计数器函数
//            {PRINT, 1, 0, 0},       // 打印第一次调用结果
//            {CALL, 1, 0, 0},        // 再次调用计数器函数
//            {PRINT, 1, 0, 0},       // 打印第二次调用结果
//            {RETURN, 0, 0, 0}       // 返回
//        },
//        std::vector<Value>{Value(0.0)}, // 常量0
//        std::vector<std::shared_ptr<FunctionProto>>{counterProto}, // 嵌套函数
//        0, 2 // 参数数, 寄存器数
//        );

//    VM vm1;
//    vm1.execute(createCounterProto);

//    std::cout << std::endl;

//    // 示例2：共享upvalue的Demo
//    std::cout << "=== 共享upvalue的Demo ===" << std::endl;

//    // 内部函数1：增加计数器
//    auto incrementProto = std::make_shared<FunctionProto>(
//        std::vector<Instruction>{
//            {GETUPVAL, 0, 0, 0},    // 获取upvalue (计数器值)
//            {LOADK, 1, 0, 0},       // R1 = 1
//            {ADD, 0, 0, 1},         // 计数器加1
//            {SETUPVAL, 0, 0, 0},    // 设置upvalue
//            {RETURN, 0, 0, 0}       // 返回计数器值
//        },
//        std::vector<Value>{Value(1.0)}, // 常量1
//        std::vector<std::shared_ptr<FunctionProto>>{}, // 无嵌套函数
//        0, 2, // 参数数, 寄存器数
//        {{true, 0}} // upvalue描述: 父函数的第0个局部变量
//        );

//    // 内部函数2：获取计数器值
//    auto getterProto = std::make_shared<FunctionProto>(
//        std::vector<Instruction>{
//            {GETUPVAL, 0, 0, 0},    // 获取upvalue (计数器值)
//            {RETURN, 0, 0, 0}       // 返回计数器值
//        },
//        std::vector<Value>{}, // 无常量
//        std::vector<std::shared_ptr<FunctionProto>>{}, // 无嵌套函数
//        0, 1, // 参数数, 寄存器数
//        {{true, 0}} // upvalue描述: 父函数的第0个局部变量
//        );

//    // 工厂函数：创建共享计数器的函数
//    auto counterFactoryProto = std::make_shared<FunctionProto>(
//        std::vector<Instruction>{
//            {LOADK, 0, 0, 0},       // R0 = 0 (初始计数器值)
//            {CLOSURE, 1, 0, 0},     // 创建增加函数
//            {CLOSURE, 2, 1, 0},     // 创建获取函数
//            {NEWTABLE, 3, 0, 0},    // 创建表
//            {LOADK, 4, 1, 0},       // R4 = "increment"
//            {SETTABLE, 3, 4, 1},    // table.increment = 增加函数
//            {LOADK, 4, 2, 0},       // R4 = "get"
//            {SETTABLE, 3, 4, 2},    // table.get = 获取函数
//            {RETURN, 3, 0, 0}       // 返回表
//        },
//        std::vector<Value>{
//            Value(0.0),                     // 0: 初始值
//            Value(std::string("increment")), // 1: "increment"
//            Value(std::string("get"))        // 2: "get"
//        },
//        std::vector<std::shared_ptr<FunctionProto>>{incrementProto, getterProto}, // 嵌套函数
//        0, 5 // 参数数, 寄存器数
//        );

//    // 使用共享计数器
//    auto useSharedCounterProto = std::make_shared<FunctionProto>(
//        std::vector<Instruction>{
//            {CLOSURE, 0, 0, 0},     // 创建工厂函数
//            {CALL, 0, 0, 0},        // 调用工厂函数
//            {MOVE, 1, 0, 0},        // R1 = 返回的表

//            // 使用共享计数器
//            {LOADK, 2, 0, 0},       // R2 = "increment"
//            {GETTABLE, 3, 1, 2},    // R3 = table.increment
//            {CALL, 3, 0, 0},        // 调用增加函数
//            {PRINT, 3, 0, 0},       // 打印结果 (1)

//            {LOADK, 2, 1, 0},       // R2 = "get"
//            {GETTABLE, 3, 1, 2},    // R3 = table.get
//            {CALL, 3, 0, 0},        // 调用获取函数
//            {PRINT, 3, 0, 0},       // 打印结果 (1)

//            {LOADK, 2, 0, 0},       // R2 = "increment"
//            {GETTABLE, 3, 1, 2},    // R3 = table.increment
//            {CALL, 3, 0, 0},        // 再次调用增加函数
//            {PRINT, 3, 0, 0},       // 打印结果 (2)

//            {LOADK, 2, 1, 0},       // R2 = "get"
//            {GETTABLE, 3, 1, 2},    // R3 = table.get
//            {CALL, 3, 0, 0},        // 再次调用获取函数
//            {PRINT, 3, 0, 0},       // 打印结果 (2)

//            {HALT, 0, 0, 0}         // 停止
//        },
//        std::vector<Value>{
//            Value(std::string("increment")), // 0: "increment"
//            Value(std::string("get"))        // 1: "get"
//        },
//        std::vector<std::shared_ptr<FunctionProto>>{counterFactoryProto}, // 嵌套函数
//        0, 4 // 参数数, 寄存器数
//        );

//    VM vm2;
//    vm2.execute(useSharedCounterProto);

}
