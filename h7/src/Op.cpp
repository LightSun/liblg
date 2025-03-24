#include <assert.h>
#include <stdio.h>

#include "common/common.h"
#include "src/Op.h"
#include "src/Value.h"
#include "src/Stack.h"
#include "src/VM.h"
#include "src/Call.h"

using namespace h7l;

void Operation::init(Position pos, enum Opcode code){
    this->pos = pos;
    this->code = code;
    auto _ = this;
    switch (_->code) {
    case LG_BEQ:
      _->as_beq.i = 0;
      _->as_beq.pc = -1;
      break;
    case LG_BGR:
      _->as_bgr.i = 0;
      _->as_bgr.pc = -1;
      break;
    case LG_BLE:
      _->as_ble.i = 0;
      _->as_ble.pc = -1;
      break;
    case LG_CALL:
      _->as_call.pc = -1;
      break;
    case LG_CP:
      _->as_cp.i = 0;
      break;
    case LG_DEC:
      _->as_dec.i = 0;
      break;
    case LG_DROP:
      _->as_drop.i = 0;
      _->as_drop.n = 1;
      break;
    case LG_JMP:
      _->as_jmp.pc = -1;
      break;
    default:
      break;
    }
}

void Operation::deinit(){
    switch (code) {
    case LG_PUSH:
      as_push.val.deinit();
      break;
    default:
      break;
    }
}
Operation::~Operation(){
    printf("Operation::~Operation\n");
    //deinit();
}
