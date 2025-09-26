#include "VM.h"

namespace h7l { namespace runtime {

void VM::execute(std::shared_ptr<FunctionProto> func){
    auto closure = std::make_shared<Closure>(func);
    callStack.push(CallFrame(closure.get(), 0, 0));
    running = true;

    // 主执行循环
    while (running && !callStack.empty()) {
        CallFrame& frame = callStack.top();
        auto closure = frame.closure;
        pc = frame.pc;

        if (pc >= (int)closure->proto->instructions.size()) {
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
            if (instr.b < (int)closure->proto->constants.size()) {
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
            auto& v1 = registers[frame.base + instr.b];
            auto& v2 = registers[frame.base + instr.c];
            auto td = m_factory.getTypeDelegate(v1.type);
            if(td != nullptr){
                registers[frame.base + instr.a] = td->add(v1, v2);
            }
            frame.pc++;
            break;
        }

        case CONCAT: {
            // 专门用于字符串连接
            auto& v1 = registers[frame.base + instr.b];
            auto& v2 = registers[frame.base + instr.c];
            auto td = m_factory.getTypeDelegate(v1.type);
            if(td != nullptr){
                registers[frame.base + instr.a] = td->concat(v1, v2);
            }
            frame.pc++;
            break;
        }

        case SUB: {
            auto& v1 = registers[frame.base + instr.b];
            auto& v2 = registers[frame.base + instr.c];
            auto td = m_factory.getTypeDelegate(v1.type);
            if(td != nullptr){
                registers[frame.base + instr.a] = td->sub(v1, v2);
            }
            frame.pc++;
            break;
        }

        case MUL: {
            auto& v1 = registers[frame.base + instr.b];
            auto& v2 = registers[frame.base + instr.c];
            auto td = m_factory.getTypeDelegate(v1.type);
            if(td != nullptr){
                registers[frame.base + instr.a] = td->mul(v1, v2);
            }
            frame.pc++;
            break;
        }

        case DIV: {
            auto& v1 = registers[frame.base + instr.b];
            auto& v2 = registers[frame.base + instr.c];
            auto td = m_factory.getTypeDelegate(v1.type);
            if(td != nullptr){
                registers[frame.base + instr.a] = td->div(v1, v2);
            }
            frame.pc++;
            break;
        }

        case CALL: {
            auto& v1 = registers[frame.base + instr.b];
            auto func = v1.getRefPtr<Closure>();
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
            auto& v1 = registers[frame.base + instr.b];
            auto func = v1.getRefPtr<CFunction>();
            if(func){
                auto val = func->func(this);
                if(!val.isVoid()){
                    registers[frame.base + instr.a] = val;
                }
            }
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
            auto& v1 = registers[frame.base + instr.b];
            if (v1.getBool()) {
                frame.pc = instr.a;
            } else {
                frame.pc++;
            }
            break;
        }

        case JMPNOT: {
            auto& v1 = registers[frame.base + instr.b];
            if (!v1.getBool()) {
                frame.pc = instr.a;
            } else {
                frame.pc++;
            }
            break;
        }

        case EQ: {
            auto& v1 = registers[frame.base + instr.b];
            auto& v2 = registers[frame.base + instr.c];
            auto td = m_factory.getTypeDelegate(v1.type);

            registers[frame.base + instr.a] = td->equals(v1, v2);
            frame.pc++;
            break;
        }

        case LT: {
            auto& v1 = registers[frame.base + instr.b];
            auto& v2 = registers[frame.base + instr.c];
            auto td = m_factory.getTypeDelegate(v1.type);
            registers[frame.base + instr.a] = td->lessThan(v1, v2);
            frame.pc++;
            break;
        }

        case LE: {
            auto& v1 = registers[frame.base + instr.b];
            auto& v2 = registers[frame.base + instr.c];
            auto td = m_factory.getTypeDelegate(v1.type);
            registers[frame.base + instr.a] = td->lessEquals(v1, v2);
            frame.pc++;
            break;
        }

        case NEWTABLE: {
            registers[frame.base + instr.a] = Value(kType_TABLE, new Table());
            frame.pc++;
            break;
        }

        case GETTABLE: {
            auto& v1 = registers[frame.base + instr.b]; //table
            auto& v2 = registers[frame.base + instr.c]; //string or int-like

            auto tab = v1.getPtr<Table>();
            if(v2.isString()){
                auto str = v2.getPtr<StringRef>();
                auto dstV = tab->get(str->toString());
                if(dstV == nullptr){
                    //TODO
                }
                registers[frame.base + instr.a] = *dstV;
            }else if(v2.isIntLike()){
                auto dstV = tab->get(v2.getAsLong());
                if(dstV == nullptr){
                    //TODO
                }
                registers[frame.base + instr.a] = *dstV;
            }
            frame.pc++;
            break;
        }

        case SETTABLE: {
            auto& tabV = registers[frame.base + instr.a];
            auto& v1 = registers[frame.base + instr.b];
            auto& v2 = registers[frame.base + instr.c];

            auto tab = tabV.getPtr<Table>();
            if(v1.isIntLike()){
                auto key = v1.getAsLong();
                tab->set(key, v2);
            }else if(v1.isString()){
                auto key = v1.getPtr<StringRef>();
                tab->set(key->toString(), v2);
            }
            frame.pc++;
            break;
        }

        case CLOSURE: {
            // 创建闭包
            if (instr.b < (int)closure->proto->nestedFunctions.size()) {
                auto nestedProto = closure->proto->nestedFunctions[instr.b];
                auto newClosure = std::make_unique<Closure>(nestedProto);

                // 设置upvalues (简化处理)
                // 这里应该根据指令的c字段来设置upvalue的来源
                // 但为了简化，我们直接从当前函数的寄存器中捕获
                for (int i = 0; i < nestedProto->upvalueCount && i < instr.c; i++) {
                    newClosure->upvalues[i] = registers[frame.base + i];
                }
                registers[frame.base + instr.a] = newClosure.release();
            } else {
                std::cerr << "Nested function index out of bounds: " << instr.b << std::endl;
            }
            frame.pc++;
            break;
        }

        case GETUPVAL: {
            if (instr.b < (int)closure->upvalues.size()) {
                registers[frame.base + instr.a] = closure->upvalues[instr.b];
            } else {
                std::cerr << "Upvalue index out of bounds: " << instr.b << std::endl;
            }
            frame.pc++;
            break;
        }

        case SETUPVAL: {
            // 设置upvalue
            if (instr.b < (int)closure->upvalues.size()) {
                closure->upvalues[instr.b] = registers[frame.base + instr.a];
            } else {
                std::cerr << "Upvalue index out of bounds: " << instr.b << std::endl;
            }
            frame.pc++;
            break;
        }

        case PRINT: {
            registers[frame.base + instr.a].print();
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


}}
