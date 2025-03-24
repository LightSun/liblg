#pragma once

#include "src/vec.h"
#include "src/Value.h"

namespace h7l {

struct Stack {
    struct lg_vec items;

    Stack(){
      lg_vec_init(&items, sizeof(Value));
    }
    ~Stack(){
      lg_vec_deinit(&items);
    }
    Value* push(){
      return (Value*)lg_vec_push(&items);
    }
    Value* peek(){
        return (Value*)lg_vec_peek(&items);
    }
    Value* pop(){
        return (Value*)lg_vec_pop(&items);
    }
    void swap(){
        Value *y = peek(), *x = y-1, tmp = *x;
        *x = *y;
        *y = tmp;
    }
    void drop(size_t i, size_t n){
        lg_vec_delete(&items, i, n);
    }
};

}


  
