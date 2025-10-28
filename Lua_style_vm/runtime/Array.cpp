#include "memory.h"
#include "runtime/Array.h"
//#include "runtime/common_base.h"

namespace h7l { namespace runtime {


void Array::printTo(std::stringstream& ss){
    ss << "[";
    if(isDataValid()){
        if(pri_is_base_type(eleType)){
            pri_base_type_array_print(ss, eleType, getDataPtr(), desc->eleCount);
        }else{
            ss << CMD_LINE;
            auto ptr = (IObjectType**)getDataPtr();
            for(size_t i = 0 ; i < desc->eleCount ; ++i){
                if(ptr[i]){
                    ptr[i]->printTo(ss);
                }else{
                    ss << "null";
                }
                if(i != desc->eleCount - 1){
                    ss << ", ";
                }
                ss << CMD_LINE;
            }
        }
    }
    ss << "]";
}

bool Array::equals(IObjectType* _oth) const{
    if(this == _oth){
        return true;
    }
    auto oth = dynamic_cast<Array*>(_oth);
    if(oth == nullptr) return false;
    if(!isDataValid() || !oth->isDataValid()){
        return false;
    }
    if(this->eleType != oth->eleType){
        return false;
    }
    if(this->desc != oth->desc){
        return false;
    }
    if(pri_is_base_type(eleType)){
        return pri_base_type_array_eqs(eleType, getDataPtr(), oth->getDataPtr(), desc->eleCount);
    }else{
        auto ptr = (IObjectType**)getDataPtr();
        auto ptr2 = (IObjectType**)oth->getDataPtr();
        for(int i = 0 ; i < (int)desc->eleCount ; ++i){
            if(ptr[i] == nullptr){
                if(ptr2[i] != nullptr){
                    return false;
                }else{
                    continue;
                }
            }
            if(!ptr[i]->equals(ptr2[i])){
                return false;
            }
        }
        return true;
    }
}
int Array::hashCode()const{
    int var = 0;
    if(pri_is_base_type(eleType)){
        auto eleSize = pri_size(eleType);
        auto ptr = getDataPtr();
        for(int i = 0 ; i < (int)desc->eleCount ; ++i){
            auto dstPtr = ptr + i * eleSize;
            var = 31 * var + base_hash(eleType, dstPtr);
        }
    }else{
        auto ptr = (IObjectType**)getDataPtr();
        for(int i = 0 ; i < (int)desc->eleCount ; ++i){
            if(ptr[i]){
                var = 31 * var + ptr[i]->hashCode();
            }else{
                var = 31 * var;
            }
        }
    }
    return var;
}


}}
