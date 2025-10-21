#pragma once

#include <unordered_map>
#include <map>
#include "runtime/Value.h"

namespace h7l { namespace runtime {

struct LoopState: public BaseObjectType<LoopState>{

    int* start {nullptr};
    int* end {nullptr};
    std::unordered_map<std::string, Value>::iterator iter_str;
    Table* tab {nullptr};
    int loopPc {-1};
    //int loopType {-1}; //fori ?

    void printTo(std::stringstream&) override{

    }
    bool equals(IObjectType*) const override{

    }
};

}}
