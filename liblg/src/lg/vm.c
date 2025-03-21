#include <stdio.h>
#include <stdlib.h>

#include "lg/call.h"
#include "lg/op.h"
#include "lg/stack.h"
#include "lg/val.h"
#include "lg/vm.h"

struct lg_vm *lg_vm_init(struct lg_vm *vm) {
  lg_vec_init(&vm->ops, sizeof(struct lg_op));
  lg_vec_init(&vm->calls, sizeof(struct lg_call));
  vm->pc = NULL;
  vm->debug = false;
  return vm;
}

void lg_vm_deinit(struct lg_vm *vm) {
  LG_VEC_DO(&vm->ops, struct lg_op *, op) {
    lg_op_deinit(op);
  }
  
  lg_vec_deinit(&vm->ops);
  lg_vec_deinit(&vm->calls);
}

struct lg_op *lg_emit(struct lg_vm *vm, struct lg_pos pos, enum lg_opcode code) {
  return lg_op_init(lg_vec_push(&vm->ops), pos, code);
}

#define LG_DISPATCH()				\
  op = vm->pc++;				\
  goto *dispatch[op->code]

void lg_exec(struct lg_vm *vm, struct lg_stack *stack, size_t start_pc) {
  static const void* dispatch[] = {NULL,
				   &&add,
				   &&beq, &&bgr, &&ble,
				   &&call, &&cp,
				   &&dec, &&drop,
				   &&jmp,
				   &&push,
				   &&ret,
				   &&stop, &&swap};
  
  struct lg_op *op = NULL;
  vm->pc = lg_vec_get(&vm->ops, start_pc);
  LG_DISPATCH();
  
 add: {
    struct lg_val y = *lg_pop(stack);
    lg_add(vm, op->pos, lg_peek(stack), y);
    lg_val_deinit(&y);
    LG_DISPATCH();
  }
 beq: {
    if ((lg_peek(stack) - op->as_beq.i)->as_int == op->as_beq.cond) {
      vm->pc = lg_vec_get(&vm->ops, op->as_beq.pc);
    }
    
    LG_DISPATCH(); 
  }
  bgr: {
    if ((lg_peek(stack) - op->as_bgr.i)->as_int > op->as_bgr.cond) {
      vm->pc = lg_vec_get(&vm->ops, op->as_bgr.pc);
    }
    
    LG_DISPATCH(); 
  }
  ble: {
      //默认直接使用结构体的匿名成员做减法(_val - op->as_ble.i)。
      struct lg_val* _val = lg_peek(stack);
      if ((_val - op->as_ble.i)->as_int < op->as_ble.cond) {
        vm->pc = lg_vec_get(&vm->ops, op->as_ble.pc);
      }
//    if ((lg_peek(stack) - op->as_ble.i)->as_int < op->as_ble.cond) {
//      vm->pc = lg_vec_get(&vm->ops, op->as_ble.pc);
//    }
    
    LG_DISPATCH(); 
  }
 call: {
      //as_call.pc: the entry of func
    lg_call(vm, lg_vec_get(&vm->ops, op->as_call.pc));
    LG_DISPATCH();
  }
 cp: {
    struct lg_val* _val = lg_peek(stack);
    lg_cp(vm, stack, *(_val - op->as_cp.i));
    LG_DISPATCH();
  }
 dec: {
    struct lg_val* _val = lg_peek(stack);
    (_val - op->as_dec.i)->as_int--;
    LG_DISPATCH();
  }
 drop: {
    lg_drop(stack, op->as_drop.i, op->as_drop.n);
    LG_DISPATCH();
  }
 jmp: {
    vm->pc = lg_vec_get(&vm->ops, op->as_jmp.pc);
    LG_DISPATCH();
  }
 push: {
    lg_clone(vm, stack, op->as_push.val);
    LG_DISPATCH();
  }
 ret: {
    struct lg_call *c = lg_vec_pop(&vm->calls);
    vm->pc = c->ret_pc;
    LG_DISPATCH();
  }
 swap: {
    lg_swap(stack);
    LG_DISPATCH();
  }

 stop: {}
}
