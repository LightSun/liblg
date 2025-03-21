#pragma once

#include "common/common.h"

namespace h7l {

struct Value;
struct VM;

struct Type{
  String id;

  virtual void add_imp(VM *vm, Value *inOut, Value* in2) = 0;

  virtual void clone_imp(VM *vm, Value* src, Value* dst) = 0;

  virtual void cp_imp(VM *vm, Value* src, Value* dst) = 0;

  virtual void deinit_imp(Value *v) = 0;

  virtual void sub_imp(VM *vm, Value *inOut, Value* in2) = 0;
};
}
