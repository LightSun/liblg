#pragma once

#include "runtime/VM.h"

namespace h7l { namespace runtime {
class VMTester{
public:
    VMTester(){
        m_vm = std::make_unique<VM>();
    }
    VM& getVM(){
        return *m_vm;
    }
     Value& getRawRegister(int index) {
         return m_vm->getRawRegister(index);
     }

private:
    std::unique_ptr<VM> m_vm;
};

}}
