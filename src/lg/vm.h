#ifndef LG_VM_H
#define LG_VM_H

#include <inttypes.h>
#include <stdbool.h>

#include "lg/op.h"
#include "lg/vec.h"

struct lg_val;

struct lg_vm {
  struct lg_vec ops;
  struct lg_vec calls;
  struct lg_op *pc;
  bool debug;
};

struct lg_vm *lg_vm_init(struct lg_vm *vm);
void lg_vm_deinit(struct lg_vm *vm);

struct lg_op *lg_emit(struct lg_vm *vm, enum lg_opcode code);
void lg_exec(struct lg_vm *vm, struct lg_stack *stack, size_t start_pc);

#endif
