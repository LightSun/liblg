#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "src/Type.h"

namespace h7l {

struct Object;

struct Value {
    bool temp {true};
    Object* objPtr {nullptr};

    Type* getType();
    void deinit();
};

}

