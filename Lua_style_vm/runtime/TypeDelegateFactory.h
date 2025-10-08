#pragma once

#include <unordered_map>
#include <memory>
#include "runtime/TypeDelegate.h"

namespace h7l { namespace runtime {


class TypeDelegateFactory{

public:
    TypeDelegateFactory();

public:
    ITypeDelegate* getTypeDelegate(int type){
        auto it = m_map.find(type);
        if(it != m_map.end()){
            return it->second.get();
        }
        return nullptr;
    }

private:
    std::unordered_map<int, std::shared_ptr<ITypeDelegate>> m_map;
};


}}
