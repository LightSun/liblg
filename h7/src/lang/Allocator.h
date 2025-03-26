#pragma once

namespace h7l {

using u32 = unsigned int;

struct Allocator{
    void* (*alloc)(u32 size);
    void* (*realloc)(void* addr, u32 size);
    void (*free)(void* addr);
};

}
