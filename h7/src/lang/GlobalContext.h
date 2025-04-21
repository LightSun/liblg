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
    Class* getClass(CString tname)const{
        auto it = m_typeMap.find(tname);
        if(it != m_typeMap.end()){
            return dynamic_cast<Class*>(it->second.get());
        }
        return nullptr;
    }
    void putType(SPType type){
        m_typeMap[type->id] = type;
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
};

}
