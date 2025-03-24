#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "src/Value.h"

namespace h7l {

struct VM;
struct Stack;

struct Position {
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

struct beq_op {
  int64_t cond;
  size_t i, pc;
};

struct bgr_op {
  int64_t cond;
  size_t i, pc;
};

struct ble_op {
  int64_t cond;
  size_t i, pc;
};

struct call_op {
  size_t pc;
};

struct cp_op {
  size_t i;
};

struct dec_op {
  size_t i;
};

struct drop_op {
  size_t i, n;
};

struct jmp_op {
  size_t pc;
};

struct push_op {
   Value val;
};

struct Operation {
    Position pos;
    enum Opcode code;

    union {
     beq_op as_beq;
     bgr_op as_bgr;
     ble_op as_ble;
     call_op as_call;
     cp_op as_cp;
     dec_op as_dec;
     drop_op as_drop;
     jmp_op as_jmp;
     push_op as_push;
    };

    ~Operation();

    void init(Position pos, enum Opcode code);
    void deinit();
};

}
