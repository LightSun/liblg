#ifndef LG_STACK_H
#define LG_STACK_H

#include "lg/vec.h"

struct lg_stack {
  struct lg_vec items;
};

struct lg_stack *lg_stack_init(struct lg_stack *_);
void lg_stack_deinit(struct lg_stack *_);

struct lg_val *lg_push(struct lg_stack *_);
struct lg_val *lg_peek(struct lg_stack *_);
struct lg_val *lg_pop(struct lg_stack *_);
void lg_swap(struct lg_stack *_);
bool lg_drop(struct lg_stack *_, size_t i, size_t n);

#endif

  
