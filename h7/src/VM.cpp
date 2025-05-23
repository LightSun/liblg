#include <stdio.h>
#include <stdlib.h>

#include "src/Call.h"
#include "src/Op.h"
#include "src/Stack.h"
#include "src/Value.h"
#include "src/VM.h"
#include "src/lang/RuntimeContext.h"

using namespace h7l;

VM::VM(Scope* scope){
    scope_ = scope;
    lg_vec_init(&ops, sizeof(struct Operation));
    lg_vec_init(&calls, sizeof(struct Call));
    pc = NULL;
    debug = false;
}
VM::~VM(){
//    LG_VEC_DO(&vm->ops, struct lg_op *, op) {
//      lg_op_deinit(op);
//    }
    lg_vec_deinit(&ops);
    lg_vec_deinit(&calls);
}

Operation *VM::emit0(Position pos, enum Opcode code){
    auto kop = (Operation*)lg_vec_push(&ops);
    kop->init(pos, code);
    return kop;
}

void VM::call(Operation* pc){
    auto call = (Call*)lg_vec_push(&calls);
    call->init(pc, this->pc);
    this->pc = pc;
}
Value* VM::copyValue(Stack* stack, Value* src){
    // auto dst = stack->push();

    // auto rc = RuntimeContext::ofVM(this, src->type);
    // if (src->type->call(kFuncOp_copy, rc.get(), dst, src)) {
    //     dst->type = src->type;
    // } else {
    //     *dst = *src;
    // }
    // return dst;
}
Value* VM::deepCopyValue(Stack* stack, Value* src){
    // auto dst = stack->push();

    // auto rc = RuntimeContext::ofVM(this, src->type);
    // if (src->type->call(kFuncOp_deepCopy, rc.get(), dst, src)) {
    //     dst->type = src->type;
    // } else {
    //     *dst = *src;
    // }
    // return dst;
}

bool VM::add(Stack* stack, Operation *op){
    //        auto y = stack->pop();
    //        add(op->pos, stack->peek(), y);
    //        y->deinit();
    auto p1 = stack->pop();
    auto inOut = stack->peek();
    //
    auto t = inOut->getType();
    auto pt = p1->getType();
    if (t->id != pt->id) {
        MED_ASSERT_F(false, "Expected type %s, actual %s", t->id.data(),
                    pt->id.data());
        return false;
    }
    auto rc = RuntimeContext::ofVM(this, t);
    auto ret = t->call(kFuncOp_add, rc.get(), inOut, p1);
    inOut->deinit();
    return ret;
}
bool VM::beq(Stack* stack, Operation* op){
    //if ((stack->peek() - op->as_beq.i)->as_int == op->as_beq.cond) {
}

bool VM::bgr(Stack* stack, Operation *op){
    // if ((stack->peek() - op->as_bgr.i)->as_int > op->as_bgr.cond) {
}

bool VM::ble(Stack* stack, Operation *op){
    //    if ((lg_peek(stack) - op->as_ble.i)->as_int < op->as_ble.cond) {
    //      vm->pc = lg_vec_get(&vm->ops, op->as_ble.pc);
    //    }
}

bool VM::copy(Stack* stack, Operation *op){
    // auto _val = stack->peek();
    // copyValue(stack, (_val - op->as_cp.i));
}

bool VM::dec(Stack* stack, Operation *op){
    // auto _val = stack->peek();
    // (_val - op->as_dec.i)->as_int--;
}


#define LG_DISPATCH()				\
  op = vm->pc++;				\
  goto *dispatch[op->code]

void VM::exec(Stack *stack, size_t start_pc){
    auto vm = this;
    static const void* dispatch[] = {NULL,
                   &&add,
                   &&beq, &&bgr, &&ble,
                   &&call, &&cp,
                   &&dec, &&drop,
                   &&jmp,
                   &&push,
                   &&ret,
                   &&stop, &&swap};

    Operation *op = NULL;
    vm->pc = (Operation*)lg_vec_get(&vm->ops, start_pc);
    LG_DISPATCH();

    add: {
//        auto y = stack->pop();
//        add(op->pos, stack->peek(), y);
//        y->deinit();
        add(stack, op);
        LG_DISPATCH();
    }
    beq: {
        if (beq(stack, op)) {
          vm->pc = (Operation*)lg_vec_get(&vm->ops, op->as_beq.pc);
        }
        LG_DISPATCH();
    }
    bgr: {
        if (bgr(stack, op)) {
          vm->pc = (Operation*)lg_vec_get(&vm->ops, op->as_bgr.pc);
        }

        LG_DISPATCH();
    }
    ble: {
        //默认直接使用结构体的匿名成员做减法(_val - op->as_ble.i)。
        if (ble(stack, op)) {
            vm->pc = (Operation*)lg_vec_get(&vm->ops, op->as_ble.pc);
        }
        //    if ((lg_peek(stack) - op->as_ble.i)->as_int < op->as_ble.cond) {
        //      vm->pc = lg_vec_get(&vm->ops, op->as_ble.pc);
        //    }

        LG_DISPATCH();
    }
    call: {
        //as_call.pc: the entry of func
        call((Operation*)lg_vec_get(&vm->ops, op->as_call.pc));
        LG_DISPATCH();
    }
    cp: {
        copy(stack, op);
        LG_DISPATCH();
    }
    dec: {
        dec(stack, op);
        LG_DISPATCH();
    }
    drop: {
       stack->drop(op->as_drop.i, op->as_drop.n);
       LG_DISPATCH();
    }
    jmp: {
        vm->pc = (Operation*)lg_vec_get(&vm->ops, op->as_jmp.pc);
        LG_DISPATCH();
    }
    push: {
        deepCopyValue(stack, &op->as_push.val);
        LG_DISPATCH();
    }
    ret: {
        auto c = (Call*)lg_vec_pop(&vm->calls);
        vm->pc = c->ret_pc;
        LG_DISPATCH();
    }
    swap: {
        stack->swap();
        LG_DISPATCH();
    }

    stop: {}
}
