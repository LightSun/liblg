#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include "src/Op.h"
#include "src/vec.h"

namespace h7l {

class Scope;
struct Value;
struct Stack;

struct VM {

    Scope* scope_ {nullptr};
    struct lg_vec ops;
    struct lg_vec calls;
    struct Operation *pc;
    bool debug;

    VM(Scope* scope);
    ~VM();

    Operation *emit0(Position pos, enum Opcode code);
    void exec(Stack *stack, size_t start_pc);
    void call(Operation *pc);
    bool add(Position pos, Value* inOut, Value* v2);
    Value* copyValue(Stack* stack, Value* src);
    Value* deepCopyValue(Stack* stack, Value* src);
};

}
