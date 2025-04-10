#ifndef LG_OP_H
#define LG_OP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "lg/pos.h"
#include "lg/val.h"

enum lg_opcode {
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

struct lg_beq_op {
  int64_t cond;
  size_t i, pc;
};

struct lg_bgr_op {
  int64_t cond;
  size_t i, pc;
};

struct lg_ble_op {
  int64_t cond;
  size_t i, pc;
};

struct lg_call_op {
  size_t pc;
};

struct lg_cp_op {
  size_t i;
};

struct lg_dec_op {
  size_t i;
};

struct lg_drop_op {
  size_t i, n;
};

struct lg_jmp_op {
  size_t pc;
};

struct lg_push_op {
  struct lg_val val;
};

struct lg_op {
  struct lg_pos pos;
  enum lg_opcode code;
  
  union {
    struct lg_beq_op as_beq;
    struct lg_bgr_op as_bgr;
    struct lg_ble_op as_ble;
    struct lg_call_op as_call;
    struct lg_cp_op as_cp;
    struct lg_dec_op as_dec;
    struct lg_drop_op as_drop;
    struct lg_jmp_op as_jmp;
    struct lg_push_op as_push;
  };
};

struct lg_vm;
struct lg_stack;

struct lg_op *lg_op_init(struct lg_op *_, struct lg_pos pos, enum lg_opcode code);
void lg_op_deinit(struct lg_op *_);

bool lg_add(struct lg_vm *vm, struct lg_pos pos, struct lg_val *x, struct lg_val y);
void lg_call(struct lg_vm *vm, struct lg_op *pc);
struct lg_val *lg_clone(struct lg_vm *vm, struct lg_stack *stack, struct lg_val src);
struct lg_val *lg_cp(struct lg_vm *vm, struct lg_stack *stack, struct lg_val src);

#endif
