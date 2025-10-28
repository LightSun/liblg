#pragma once

#include "runtime/utils/ConstantPool.h"

namespace h7l { namespace runtime {

class ModuleConstantPool{
public:

    ConstantPoolPlan& at(int mod){
        return moduleMap_.at(mod);
    }
    const ConstantPoolPlan& at(int mod)const{
        return moduleMap_.at(mod);
    }
    ConstantPoolPlan& operator[](int mod){
        return moduleMap_[mod];
    }

private:
    std::vector<ConstantPoolPlan> moduleMap_;
};

}}
