#pragma once

#include "common/common.h"

namespace h7l {

struct Value;
struct VM;

struct Type{
  String id;

  virtual bool add_impl(VM *vm, Value *inOut, Value* in2) = 0;

  virtual bool copy_impl(VM *vm, Value* src, Value* dst) = 0;

  virtual bool deep_copy_impl(VM *vm, Value* src, Value* dst) = 0;

  virtual bool deinit_impl(Value *v) = 0;

  virtual bool sub_impl(VM *vm, Value *inOut, Value* in2) = 0;
};
}
