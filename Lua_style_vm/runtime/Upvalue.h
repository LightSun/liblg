#pragma once

#include "runtime/Value.h"

namespace h7l { namespace runtime {

struct Upvalue {
    Value* value;           // 指向值的指针（可能是栈位置或堆位置）
    Value closedValue;      // 当upvalue关闭时存储的值
    bool isClosed;          // 是否已关闭（不再指向栈）

    Upvalue(Value* v) : value(v), isClosed(false) {}

    Value* get() {
        if (isClosed) {
            return &closedValue;
        }
        return value;
    }

    void set(const Value& v) {
        if (isClosed) {
            closedValue = v;
        } else {
            *value = v;
        }
    }
    // 关闭upvalue（当函数返回时）
    void close() {
        if (!isClosed) {
            closedValue = *value;
            isClosed = true;
            value = nullptr;
        }
    }
};

}}
