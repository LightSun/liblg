#include <stdlib.h>
#include "Allocator.h"

namespace h7l {

static Allocator* sAllocator = nullptr;

void setGlobalAllocator(Allocator* aoc){
    sAllocator->alloc = aoc->alloc;
    sAllocator->realloc = aoc->realloc;
    sAllocator->free = aoc->free;
}
Allocator* getGlobalAllocator(){
    if(!sAllocator){
        sAllocator = new Allocator();
        sAllocator->free = free;
        sAllocator->alloc = malloc;
        sAllocator->realloc = realloc;
    }
    return sAllocator;
}

}
