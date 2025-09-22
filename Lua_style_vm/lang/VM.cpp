#include "VM.h"
#include "Object.h"

using namespace h7l;

void VM::execute(Scope* scope,std::shared_ptr<FunctionProto> func){
    currentFunction_ = func;
    pc_ = 0;
    running_ = true;
    auto& currentFunction = currentFunction_;
    auto& registers = registers_;
    auto& pc = pc_;
    //
    while (running_.load(std::memory_order_acquire)
           && pc < (int)func->instructions.size()) {
        auto& instr = func->instructions[pc_];
        switch (instr.opcode) {
        case LOADK: {
            if (instr.b < (int)currentFunction->constants.size()) {
                registers[instr.a] = currentFunction->constants[instr.b];
            } else {
                std::cerr << "Constant index out of bounds: " << instr.b << std::endl;
            }
            pc++;
        }break;

        case LOADBOOL: {
            auto v = static_cast<bool>(instr.b);
            auto obj = Object::newPrimitive(scope, kPriType_BOOL);
            obj->wrapOfPrimitive(v);
            registers[instr.a] = Value::makeStrongDirect(obj);
            pc++;
        }break;

        case MOVE: {
            registers[instr.a] = registers[instr.b];
            pc++;
        }break;

        case ADD: {

        }break;

        case SUB: {

        }break;
        case MUL: {

        }break;
        case DIV: {

        }break;

        case CONCAT: {

        }break;

        case CALL: {

        }break;
        case RETURN: {

        }break;
            //-----

        case JMP: {

        }break;
        case JMPIF: {

        }break;
        case JMPNOT: {

        }break;
        case EQ: {

        }break;
        case LT: {

        }break;
        case LE: {

        }break;
        //NEWTABLE,SETTABLE?

        case HALT: {
            running_ = false;
            break;
        }

        default: {
            std::cerr << "Unknown opcode: " << instr.opcode << std::endl;
            pc_++;
            break;
        }
        }
    }
}
