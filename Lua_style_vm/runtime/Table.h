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
    std::map<int, Value> array;

    int getArraySize()const{
        auto it = fields.begin();
        return array.size();
    }
    void setArraySize(int size){
        {
            List<int> rmKeys;
            for(auto& p : array){
                if(p.first >= size){
                    rmKeys.push_back(p.first);
                }
            }
            for(auto& k : rmKeys){
                array.erase(k);
            }
        }
        for(int i = 0 ; i < size ; ++i){
            auto it = array.find(i);
            if(it == array.end()){
                it->second = Value();
            }
        }
    }
    //1d arr
    bool getArrayShapeDynamic(List<int>& shape){
        bool dynamic = false;
        int size = array.size();
        for(int i = 0 ; i < size ; ++i){
            auto& v = array[i];
            if(v.isIntLike()){
                shape.push_back(v.getAsLong());
            }else{
                if(!dynamic){
                    dynamic = true;
                }
                shape.push_back(-1);
            }
        }
        return dynamic;
    }

    Value* get(const std::string& key) {
        auto it = fields.find(key);
        if (it != fields.end()) {
            return &it->second;
        }
        return nullptr;
    }
    Value* get(size_t index) {
        auto it = array.find(index);
        if (it != array.end()) {
            return &it->second;
        }
        return nullptr;
    }
    void set(const std::string& key, const Value& value) {
        fields[key] = value;
    }
    void set(size_t index, const Value& value) {
        array[index] = value;
    }
    bool contains(const std::string& key) {
        return fields.find(key) != fields.end();
    }
    bool contains(size_t index) {
        return array.find(index) != array.end();
    }
    Table* copy(){
        Table* tab = new Table();
        tab->fields = this->fields;
        tab->array = this->array;
        return tab;
    }
    void removeSame(Table* other){
        for(auto& [k,v]: other->fields){
            fields.erase(k);
        }
        for(auto& [k,v]: other->array){
            array.erase(k);
        }
    }
    Table* merge(Table* other){
        Table* tab = new Table();
        mergeTo(tab);
        other->mergeTo(tab);
        return tab;
    }
    void mergeTo(Table* other){
        for(auto& [k,v] : fields){
            other->set(k, v);
        }
        for(auto& [k,v] : array){
            other->set(k, v);
        }
    }

    //---------------
    void printTo(std::stringstream& ss)override{
        if(array.empty()){
            ss << "{";
            for(auto& [k,v] : fields){
                ss << k << ": " << v;
            }
            ss << "}";
        }else{
            ss << "[";
            for(auto& p: array){
                if(p.first != array.begin()->first){
                    ss << ",";
                }
                ss << p.second;
            }
            ss << "]";
        }
    }
    bool equals(IObjectType* oth) const override{
        Table* o1 = (Table*)oth;
        auto& o = *o1;
        if(array.size() != o1->array.size()){
            return false;
        }
        if(fields.size() != fields.size()){
            return false;
        }
        for(auto& [k,v] : fields){
            auto v2 = o.get(k);
            if(v2 == nullptr){
                return false;
            }
            if(!v.equals(*v2)){
                return false;
            }
        }
        for(auto& [k,v] : array){
            auto v2 = o.get(k);
            if(v2 == nullptr){
                return false;
            }
            if(!v.equals(*v2)){
                return false;
            }
        }
        return true;
    }
};


}}
