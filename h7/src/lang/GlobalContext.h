#pragma once

#include <common/common.h>
#include "src/lang/Class.h"
#include "src/lang/Allocator.h"

namespace h7l {

//cache: type, class, string
class GlobalContext
{
public:
    GlobalContext(){}

    void setAllocator(const Allocator& alloc){
        m_allocator = alloc;
    }
    Allocator& getAllocator(){
        return m_allocator;
    }
    Type* getType(CString tname)const{
        auto it = m_typeMap.find(tname);
        if(it != m_typeMap.end()){
            return it->second.get();
        }
        return nullptr;
    }
    Class* getClass(CString tname)const{
        auto it = m_classMap.find(tname);
        if(it != m_classMap.end()){
            return it->second.get();
        }
        return nullptr;
    }
    void putType(std::unique_ptr<Type> type){
        String name = type->id;
        m_typeMap[name] = std::move(type);
    }
    void putClass(std::unique_ptr<Class> cls){
        String name = cls->id;
        m_classMap[name] = std::move(cls);
    }
private:
    Allocator m_allocator;
    std::map<String, std::unique_ptr<Type>> m_typeMap;
    std::map<String, std::unique_ptr<Class>> m_classMap;
};

}
