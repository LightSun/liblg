#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include "src/Op.h"
#include "src/vec.h"

namespace h7l {

class GlobalContext;
struct Value;
struct Stack;

struct VM {

    GlobalContext* gCtx {nullptr};
    struct lg_vec ops;
    struct lg_vec calls;
    struct Operation *pc;
    bool debug;

    VM(GlobalContext* gc);
    ~VM();

    Operation *emit0(Position pos, enum Opcode code);
    void exec(Stack *stack, size_t start_pc);
    void call(Operation *pc);
    bool add(VM* vm, Position pos, Value* inOut, Value* v2);
    Value* copyValue(Stack* stack, Value* src);
    Value* deepCopyValue(Stack* stack, Value* src);
};

}
