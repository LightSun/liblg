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
    ~GlobalContext(){
        auto it = m_typeMap.begin();
        for(; it != m_typeMap.end(); ++it){
            H7L_FREE(it->second);
        }
    }

    Type* getType(CString tname)const{
        auto it = m_typeMap.find(tname);
        if(it != m_typeMap.end()){
            return it->second;
        }
        return nullptr;
    }
    Class* getClass(CString tname)const{
        auto it = m_typeMap.find(tname);
        if(it != m_typeMap.end()){
            return dynamic_cast<Class*>(it->second);
        }
        return nullptr;
    }
    void putType(Type* type){
        m_typeMap[type->id] = std::move(type);
    }
    template<typename T>
    Type* putRawType(){
        auto crc = (T*)H7L_ALLOC(sizeof(T));
        crc->init();
        putType(crc);
        return crc;
    }
private:
    std::map<String, Type*> m_typeMap;
};

}
