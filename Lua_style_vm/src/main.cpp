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

// 值类型：可以存储数字、字符串、布尔值、表、函数或nil
using Value = std::variant<double, std::string, bool, std::shared_ptr<Table>,
                           std::function<void()>, std::nullptr_t>;

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
// 指令结构: opcode, dstReg. op1, op2
struct Instruction {
    OpCode opcode;
    int a;  // 通常用于目标寄存器
    int b;  // 操作数1或常量索引
    int c;  // 操作数2
    
    Instruction(OpCode op, int a = 0, int b = 0, int c = 0) 
        : opcode(op), a(a), b(b), c(c) {}
};

// 函数原型: 指令，常量，参数个数，寄存器个数
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

// 虚拟机状态
class VM {
private:
    std::vector<Value> registers;  // 寄存器数组
    std::stack<std::vector<Value>> callStack;  // 调用栈，保存寄存器状态
    std::stack<int> returnPc;      // 返回地址
    int pc;                        // 程序计数器
    bool running;                  // 运行标志
    
    // 当前执行的函数原型
    std::shared_ptr<FunctionProto> currentFunction;
    
    // 辅助函数：获取值作为数字
    double getAsNumber(const Value& v) {
        if (std::holds_alternative<double>(v)) {
            return std::get<double>(v);
        }
        std::cerr << "Type error: expected number\n";
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
        std::cerr << "Type error: expected table\n";
        return nullptr;
    }
    
    // 辅助函数：值转换为字符串
    std::string valueToString(const Value& v) {
        if (std::holds_alternative<double>(v)) {
            return std::to_string(std::get<double>(v));
        } else if (std::holds_alternative<std::string>(v)) {
            return std::get<std::string>(v);
        } else if (std::holds_alternative<bool>(v)) {
            return std::get<bool>(v) ? "true" : "false";
        } else if (std::holds_alternative<std::shared_ptr<Table>>(v)) {
            return "[table]";
        } else if (std::holds_alternative<std::nullptr_t>(v)) {
            return "nil";
        } else {
            return "[function]";
        }
    }

public:
    VM() : pc(0), running(false) {
        registers.resize(20);  // 初始寄存器数量
    }
    
    // 执行函数
    void execute(std::shared_ptr<FunctionProto> func) {
        currentFunction = func;
        pc = 0;
        running = true;
        
        // 确保有足够的寄存器
        if ((int)registers.size() < func->numRegisters) {
            registers.resize(func->numRegisters);
        }
        
        // 主执行循环
        while (running && pc < (int)func->instructions.size()) {
            const Instruction& instr = func->instructions[pc];
            // std::cout << "Executing instruction at pc=" << pc << ": " << instr.opcode << std::endl;
            
            switch (instr.opcode) {
                case LOADK: {
                    if (instr.b < (int)currentFunction->constants.size()) {
                        registers[instr.a] = currentFunction->constants[instr.b];
                    } else {
                        std::cerr << "Constant index out of bounds: " << instr.b << std::endl;
                    }
                    pc++;
                    break;
                }
                
                case LOADBOOL: {
                    registers[instr.a] = static_cast<bool>(instr.b);
                    pc++;
                    break;
                }
                
                case MOVE: {
                    registers[instr.a] = registers[instr.b];
                    pc++;
                    break;
                }
                
                case ADD: {
                    // 支持数字和字符串的加法
                    if (std::holds_alternative<double>(registers[instr.b]) && 
                        std::holds_alternative<double>(registers[instr.c])) {
                        double val1 = std::get<double>(registers[instr.b]);
                        double val2 = std::get<double>(registers[instr.c]);
                        registers[instr.a] = val1 + val2;
                    } else {
                        // 如果有一个操作数是字符串，则进行字符串连接
                        std::string str1 = getAsString(registers[instr.b]);
                        std::string str2 = getAsString(registers[instr.c]);
                        registers[instr.a] = str1 + str2;
                    }
                    pc++;
                    break;
                }
                
                case CONCAT: {
                    // 专门用于字符串连接
                    std::string str1 = getAsString(registers[instr.b]);
                    std::string str2 = getAsString(registers[instr.c]);
                    registers[instr.a] = str1 + str2;
                    pc++;
                    break;
                }
                
                case SUB: {
                    double val1 = getAsNumber(registers[instr.b]);
                    double val2 = getAsNumber(registers[instr.c]);
                    registers[instr.a] = val1 - val2;
                    pc++;
                    break;
                }
                
                case MUL: {
                    double val1 = getAsNumber(registers[instr.b]);
                    double val2 = getAsNumber(registers[instr.c]);
                    registers[instr.a] = val1 * val2;
                    pc++;
                    break;
                }
                
                case DIV: {
                    double val1 = getAsNumber(registers[instr.b]);
                    double val2 = getAsNumber(registers[instr.c]);
                    if (val2 != 0) {
                        registers[instr.a] = val1 / val2;
                    } else {
                        std::cerr << "Division by zero!\n";
                        registers[instr.a] = 0.0;
                    }
                    pc++;
                    break;
                }
                
                case CALL: {
                    // 保存当前状态
                    callStack.push(registers);
                    returnPc.push(pc + 1);
                    
                    // 设置新的执行环境
                    // 这里简化处理，实际Lua虚拟机有更复杂的调用机制
                    pc = instr.b;  // 跳转到函数开始位置
                    break;
                }
                
                case RETURN: {
                    if (returnPc.empty()) {
                        running = false;
                    } else {
                        // 恢复调用者状态
                        registers = callStack.top();
                        callStack.pop();
                        pc = returnPc.top();
                        returnPc.pop();
                    }
                    break;
                }
                
                case JMP: {
                    pc = instr.a;
                    break;
                }
                
                case JMPIF: {
                    if (getAsBool(registers[instr.b])) {
                        pc = instr.a;
                    } else {
                        pc++;
                    }
                    break;
                }
                
                case JMPNOT: {
                    if (!getAsBool(registers[instr.b])) {
                        pc = instr.a;
                    } else {
                        pc++;
                    }
                    break;
                }
                
                case EQ: {
                    // 支持多种类型的相等比较
                    bool result = false;
                    const Value& val1 = registers[instr.b];
                    const Value& val2 = registers[instr.c];
                    
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
                    
                    registers[instr.a] = result;
                    pc++;
                    break;
                }
                
                case LT: {
                    // 支持数字和字符串的小于比较
                    bool result = false;
                    const Value& val1 = registers[instr.b];
                    const Value& val2 = registers[instr.c];
                    
                    if (std::holds_alternative<double>(val1) && std::holds_alternative<double>(val2)) {
                        result = std::get<double>(val1) < std::get<double>(val2);
                    } else if (std::holds_alternative<std::string>(val1) && std::holds_alternative<std::string>(val2)) {
                        result = std::get<std::string>(val1) < std::get<std::string>(val2);
                    } else {
                        std::cerr << "Type error: cannot compare these types with LT\n";
                    }
                    
                    registers[instr.a] = result;
                    pc++;
                    break;
                }
                
                case LE: {
                    // 支持数字和字符串的小于等于比较
                    bool result = false;
                    const Value& val1 = registers[instr.b];
                    const Value& val2 = registers[instr.c];
                    
                    if (std::holds_alternative<double>(val1) && std::holds_alternative<double>(val2)) {
                        result = std::get<double>(val1) <= std::get<double>(val2);
                    } else if (std::holds_alternative<std::string>(val1) && std::holds_alternative<std::string>(val2)) {
                        result = std::get<std::string>(val1) <= std::get<std::string>(val2);
                    } else {
                        std::cerr << "Type error: cannot compare these types with LE\n";
                    }
                    
                    registers[instr.a] = result;
                    pc++;
                    break;
                }
                
                case NEWTABLE: {
                    registers[instr.a] = std::make_shared<Table>();
                    pc++;
                    break;
                }
                
                case GETTABLE: {
                    auto table = getAsTable(registers[instr.b]);
                    if (table) {
                        // 根据键的类型从表中获取值
                        const Value& key = registers[instr.c];
                        if (std::holds_alternative<std::string>(key)) {
                            std::string keyStr = std::get<std::string>(key);
                            if (table->contains(keyStr)) {
                                // 这里简化处理，直接将any转换为Value
                                // 实际实现需要更复杂的类型转换
                                std::any value = table->get(keyStr);
                                try {
                                    // 尝试转换为double
                                    registers[instr.a] = std::any_cast<double>(value);
                                } catch (...) {
                                    try {
                                        // 尝试转换为string
                                        registers[instr.a] = std::any_cast<std::string>(value);
                                    } catch (...) {
                                        // 其他类型暂时设置为nil
                                        registers[instr.a] = nullptr;
                                    }
                                }
                            } else {
                                registers[instr.a] = nullptr;
                            }
                        } else if (std::holds_alternative<double>(key)) {
                            int index = static_cast<int>(std::get<double>(key));
                            if (table->contains(index)) {
                                std::any value = table->get(index);
                                try {
                                    registers[instr.a] = std::any_cast<double>(value);
                                } catch (...) {
                                    try {
                                        registers[instr.a] = std::any_cast<std::string>(value);
                                    } catch (...) {
                                        registers[instr.a] = nullptr;
                                    }
                                }
                            } else {
                                registers[instr.a] = nullptr;
                            }
                        } else {
                            std::cerr << "Table key must be string or number\n";
                            registers[instr.a] = nullptr;
                        }
                    } else {
                        registers[instr.a] = nullptr;
                    }
                    pc++;
                    break;
                }
                
                case SETTABLE: {
                    auto table = getAsTable(registers[instr.a]);
                    if (table) {
                        // 根据键的类型设置表中的值
                        const Value& key = registers[instr.b];
                        const Value& value = registers[instr.c];
                        
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
                    pc++;
                    break;
                }
                
                case PRINT: {
                    std::cout << valueToString(registers[instr.a]) << std::endl;
                    pc++;
                    break;
                }
                
                case HALT: {
                    running = false;
                    break;
                }
                
                default: {
                    std::cerr << "Unknown opcode: " << instr.opcode << std::endl;
                    pc++;
                    break;
                }
            }
        }
    }
    
    // 获取寄存器值（用于测试和调试）
    Value getRegister(int index) {
        if (index >= 0 && index < (int)registers.size()) {
            return registers[index];
        }
        return nullptr;
    }
};



static void test_string();
static void test_table();
static void test_mix();
static void test_loop();
extern void main_demo2();

int main() {
    //test_string();
    //test_table();
    //test_loop();
    main_demo2();
    return 0;
}

using String = std::string;

static void test_string(){
    // 示例1：字符串操作
    std::cout << "=== 字符串操作示例 ===" << std::endl;
    std::vector<Instruction> stringInstructions = {
        {LOADK, 0, 0, 0},  // R0 = "Hello"
        {LOADK, 1, 1, 0},  // R1 = " "
        {LOADK, 2, 2, 0},  // R2 = "World"
        {CONCAT, 3, 0, 1}, // R3 = R0 + R1 ("Hello ")
        {CONCAT, 4, 3, 2}, // R4 = R3 + R2 ("Hello World")
        {PRINT, 4, 0, 0},  // 打印R4
        {HALT, 0, 0, 0}    // 停止
    };

    std::vector<Value> stringConstants = {
        String("Hello"),
        String(" "),
        String("World")
    };

    auto stringFunc = std::make_shared<FunctionProto>(stringInstructions, stringConstants, 0, 5);
    VM vm1;
    vm1.execute(stringFunc);

    std::cout << std::endl;
}


static void test_table(){
    // 示例2：表操作
    std::cout << "=== 表操作示例 ===" << std::endl;
    std::vector<Instruction> tableInstructions = {
        {NEWTABLE, 0, 0, 0},  // R0 = {}
        {LOADK, 1, 0, 0},     // R1 = "name"
        {LOADK, 2, 1, 0},     // R2 = "Alice"
        {SETTABLE, 0, 1, 2},  // R0[R1] = R2 (R0.name = "Alice")
        {LOADK, 3, 2, 0},     // R3 = "age"
        {LOADK, 4, 3, 0},     // R4 = 25
        {SETTABLE, 0, 3, 4},  // R0[R3] = R4 (R0.age = 25)
        {LOADK, 5, 4, 0},     // R5 = 1
        {LOADK, 6, 5, 0},     // R6 = "first"
        {SETTABLE, 0, 5, 6},  // R0[1] = "first"

        // 获取值并打印
        {LOADK, 7, 0, 0},     // R7 = "name"
        {GETTABLE, 8, 0, 7},  // R8 = R0[R7] (R0.name)
        {PRINT, 8, 0, 0},     // 打印R8

        {LOADK, 9, 2, 0},     // R9 = "age"
        {GETTABLE, 10, 0, 9}, // R10 = R0[R9] (R0.age)
        {PRINT, 10, 0, 0},    // 打印R10

        {LOADK, 11, 6, 0},    // R11 = 1
        {GETTABLE, 12, 0, 11},// R12 = R0[1]
        {PRINT, 12, 0, 0},    // 打印R12

        {HALT, 0, 0, 0}       // 停止
    };

    std::vector<Value> tableConstants = {
        String("name"),
        String("Alice"),
        String("age"),
        25.0,
        1.0,
        String("first"),
        1.0
    };

    auto tableFunc = std::make_shared<FunctionProto>(tableInstructions, tableConstants, 0, 13);
    VM vm2;
    vm2.execute(tableFunc);

    std::cout << std::endl;
}

static void test_mix(){
    // 示例3：混合类型操作
    std::cout << "=== 混合类型操作示例 ===" << std::endl;
    std::vector<Instruction> mixedInstructions = {
        {LOADK, 0, 0, 0},  // R0 = "The answer is: "
        {LOADK, 1, 1, 0},  // R1 = 42
        {ADD, 2, 0, 1},    // R2 = R0 + R1 (字符串和数字连接)
        {PRINT, 2, 0, 0},  // 打印R2

        {LOADK, 3, 2, 0},  // R3 = 10
        {LOADK, 4, 3, 0},  // R4 = 3
        {DIV, 5, 3, 4},    // R5 = R3 / R4 (10 / 3)
        {PRINT, 5, 0, 0},  // 打印R5

        {HALT, 0, 0, 0}    // 停止
    };

    std::vector<Value> mixedConstants = {
        String("The answer is: "),
        42.0, 10.0, 3.0
    };

    auto mixedFunc = std::make_shared<FunctionProto>(mixedInstructions, mixedConstants, 0, 6);
    VM vm3;
    vm3.execute(mixedFunc);
}
static void test_loop(){
    // 示例5：循环调用Demo - 计算斐波那契数列
    std::cout << "=== 循环调用Demo：斐波那契数列 ===" << std::endl;

    // 计算斐波那契数列的前10个数
    // 伪代码：
    // a = 0
    // b = 1
    // for i = 1 to 10 do
    //   print(a)
    //   next = a + b
    //   a = b
    //   b = next
    // end

    std::vector<Instruction> loopInstructions = {
        // 初始化变量
        {LOADK, 0, 0, 0},   // R0 = a = 0
        {LOADK, 1, 1, 0},   // R1 = b = 1
        {LOADK, 2, 2, 0},   // R2 = i = 1 (计数器)
        {LOADK, 3, 3, 0},   // R3 = 10 (循环上限)
        {LOADK, 4, 1, 0},   // R4 = 1 (增量)

        // 循环开始
        {LE, 5, 2, 3},      // R5 = (i <= 10)
        {JMPNOT, 16, 5, 0}, // 如果 i > 10，跳转到结束 (指令16)

        // 循环体
        {PRINT, 0, 0, 0},   // 打印 a
        {ADD, 6, 0, 1},     // R6 = a + b (next)
        {MOVE, 0, 1, 0},    // a = b
        {MOVE, 1, 6, 0},    // b = next
        {ADD, 2, 2, 4},     // i = i + 1
        {JMP, 5, 0, 0},     // 跳回循环开始 (指令5)

        // 循环结束
        {LOADK, 7, 4, 0},   // R7 = "Done!"
        {PRINT, 7, 0, 0},   // 打印 "Done!"
        {HALT, 0, 0, 0}     // 停止
    };

    std::vector<Value> loopConstants = {
        Value(0.0),        // 0: a的初始值
        Value(1.0),        // 1: b的初始值和增量
        Value(1.0),        // 2: i的初始值
        Value(10.0),       // 3: 循环上限
        Value(std::string("Done!"))  // 4: 完成消息
    };

    auto loopFunc = std::make_shared<FunctionProto>(loopInstructions,
                                                    loopConstants, 0, 8);
    VM vm5;
    vm5.execute(loopFunc);

    std::cout << std::endl;
}
