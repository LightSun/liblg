#pragma once

#include <stdlib.h>

//#define H7L_ALLOC(size) getGlobalAllocator()->alloc(size)
//#define H7L_REALLOC(addr,size) getGlobalAllocator()->realloc(addr,size)
//#define H7L_FREE(addr) getGlobalAllocator()->free(addr)

namespace h7l {

using u32 = unsigned int;

struct Allocator{
    void* (*alloc)(size_t size);
    void* (*realloc)(void* addr, size_t size);
    void (*free)(void* addr);
};

void setGlobalAllocator(Allocator* aoc);
Allocator* getGlobalAllocator();

}
