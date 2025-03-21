#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "src/Value.h"

namespace h7l {

struct VM;

struct h7l_Pos {
  int row, col;
};

enum Opcode {
    LG_NOP = 0,

    LG_ADD,
    LG_BEQ, //Branch to $label if the stack value at $offset is equal to $value.

    LG_BGR, //Branch to $label if the stack value at $offset is greater than $value.

    LG_BLE, //Branch to $label if the stack value at $offset is less than $value.

    LG_CALL, //Push the program counter on the call stack and jump to $label.

    LG_CP,  //Push a copy of the value at stack $offset.

    LG_DEC, //Decrease the value at stack $offset by one.

    //从栈上，指定位置移除一定个数.
    LG_DROP, //Drop $count items from stack $offset.

    LG_JMP,  //Jump to $label.

    LG_PUSH, //Push $value on the stack.

    LG_RET,  //Return from the current call.

    LG_STOP, //Stop execution.

    LG_SWAP, //Swap the top two items on the stack.

    LG_OP_MAX
};

struct h7l_beq_op {
  int64_t cond;
  size_t i, pc;
};

struct h7l_bgr_op {
  int64_t cond;
  size_t i, pc;
};

struct h7l_ble_op {
  int64_t cond;
  size_t i, pc;
};

struct h7l_call_op {
  size_t pc;
};

struct h7l_cp_op {
  size_t i;
};

struct h7l_dec_op {
  size_t i;
};

struct h7l_drop_op {
  size_t i, n;
};

struct h7l_jmp_op {
  size_t pc;
};

struct h7l_push_op {
   Value val;
};

struct h7l_op {
    h7l_Pos pos;
    enum Opcode code;

    union {
     h7l_beq_op as_beq;
     h7l_bgr_op as_bgr;
     h7l_ble_op as_ble;
     h7l_call_op as_call;
     h7l_cp_op as_cp;
     h7l_dec_op as_dec;
     h7l_drop_op as_drop;
     h7l_jmp_op as_jmp;
     h7l_push_op as_push;
    };

    void init(h7l_Pos pos, enum Opcode code);
    bool add(VM* vm, h7l_Pos pos, Value* v1, Value* v2);
    void call(VM* vm);
    //void call(VM* vm);
};

//struct lg_op *lg_op_init(struct lg_op *_, struct lg_pos pos, enum lg_opcode code);
//void lg_op_deinit(struct lg_op *_);

//bool lg_add(struct lg_vm *vm, struct lg_pos pos, struct lg_val *x, struct lg_val y);
//void lg_call(struct lg_vm *vm, struct lg_op *pc);
//struct lg_val *lg_clone(struct lg_vm *vm, struct lg_stack *stack, struct lg_val src);
//struct lg_val *lg_cp(struct lg_vm *vm, struct lg_stack *stack, struct lg_val src);

}
