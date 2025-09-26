#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <memory>
#include <functional>

#include "runtime/Value.h"
#include "runtime/String.h"

namespace h7l { namespace runtime {

struct Table: public BaseObjectType<Table>
{
    std::unordered_map<std::string, Value> fields;
    std::vector<Value> arrayPart;

    Value* get(const std::string& key) {
        if (fields.find(key) != fields.end()) {
            return &fields[key];
        }
        return nullptr;
    }
    Value* get(size_t index) {
        if (index < arrayPart.size()) {
            return &arrayPart[index];
        }
        return nullptr;
    }
    void set(const std::string& key, const Value& value) {
        fields[key] = value;
    }
    void set(size_t index, const Value& value) {
        arrayPart[index] = value;
    }
    bool contains(const std::string& key) {
        return fields.find(key) != fields.end();
    }
    bool contains(size_t index) {
        return index >=0 && index < arrayPart.size();
    }
    Table* merge(Table* other){
        Table* tab = new Table();
        mergeTo(tab);
        other->mergeTo(tab);
        return tab;
    }
    void mergeTo(Table* other){
        if(arrayPart.empty()){
            for(auto& [k,v] : fields){
                other->set(k, v);
            }
        }else{
            auto& arr = other->arrayPart;
            arr.reserve(arr.size() + arrayPart.size());
            arr.insert(arr.end(), arrayPart.begin(), arrayPart.end());
        }
    }

    //---------------
    void printTo(std::ostream& ss)override{
        if(arrayPart.empty()){
            ss << "{";
            for(auto& [k,v] : fields){
                ss << k << ": " << v;
            }
            ss << "}";
        }else{
            ss << "[";
            int size = arrayPart.size();
            for(int i = 0 ; i < size; ++i){
                ss << arrayPart[i];
                if(i != size - 1){
                    ss << ", ";
                }
            }
            ss << "]";
        }
    }
    bool equals(IObjectType* oth) const override{
        Table* o1 = (Table*)oth;
        auto& o = *o1;
        if(arrayPart.size() != o1->arrayPart.size()){
            return false;
        }
        if(fields.size() != fields.size()){
            return false;
        }
        if(arrayPart.empty()){
            for(auto& [k,v] : fields){
                auto v2 = o.get(k);
                if(v2 == nullptr){
                    return false;
                }
                if(!v.equals(*v2)){
                    return false;
                }
            }
        }else{
            int size = arrayPart.size();
            for(int i = 0 ; i < size ; ++i){
                auto& v = arrayPart[i];
                auto& v2 = o.arrayPart[i];
                if(!v.equals(v2)){
                    return false;
                }
            }
        }
        return true;
    }
};


}}
