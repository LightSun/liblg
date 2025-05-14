#pragma once

#include <common/common.h>
#include "src/lang/Class.h"
#include "src/lang/Allocator.h"

namespace h7l {

//cache: type, class, string
class GlobalContext
{
public:
    using SPType = std::shared_ptr<Type>;
    GlobalContext();
    ~GlobalContext(){
    }
    Type* getType(CString tname)const{
        auto it = m_typeMap.find(tname);
        if(it != m_typeMap.end()){
            return it->second.get();
        }
        return nullptr;
    }
    //TODO opt to static array-ref+index
    Type* getPrimitiveType(int priType){
        auto it = m_priTypeMap.find(priType);
        if(it != m_priTypeMap.end()){
            return it->second;
        }
        return nullptr;
    }
    Class* getClass(CString tname)const{
        auto it = m_typeMap.find(tname);
        if(it != m_typeMap.end()){
            return !it->second->isPrimetiveType()? (Class*)it->second.get() : nullptr;
        }
        return nullptr;
    }
    void putType(SPType type){
        m_typeMap[type->id] = type;
        if(type->isPrimetiveType()){
            m_priTypeMap[type->priType] = type.get();
        }
    }
    template<typename T>
    Type* putRawType(){
        auto crc = std::make_shared<T>();
        //crc->init();
        putType(crc);
        return (Type*)crc.get();
    }
private:
    std::unordered_map<String, SPType> m_typeMap;
    std::unordered_map<int, Type*> m_priTypeMap;
};

}
