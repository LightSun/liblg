#pragma once

#include "runtime/utils/StringInternPool.h"
#include "runtime/Value.h"

namespace h7l { namespace runtime {
//struct ValueEquals{
//    bool operator()(const Value& v1, const Value& v2)const{
//        if(v1.type == v2.type){
//            return true;
//        }
//        if(v1.type == kType_NULL || v2.type == kType_NULL){
//            return false;
//        }
//        return v1.equals(v2);
//    }
//};

class TypePool{
public:
    TypePool():TypePool(16, 0.75f){}
    TypePool(size_t initial_capacity, double load_factor)
        :constant_map_(initial_capacity, load_factor){
    }

    size_t add(const Value& val){
        std::unique_lock lock(mutex_);
        //
        auto hash = val.hashCode();
        Item item(val.type, hash);
        //
        auto ev = constant_map_.find(item);
        if(ev != nullptr){
            if(ev->index < pool_.size()){
                return ev->index;
            }
            //hash conflict
        }
        item.index = pool_.size();
        pool_.push_back(val);
        constant_map_.insert(item);
        return item.index;
    }
    const Value& getAt(size_t index)const{
        std::shared_lock lock(mutex_);
        return pool_.at(index);
    }
    Value& getAt(size_t index){
        std::shared_lock lock(mutex_);
        return pool_.at(index);
    }
    size_t size()const{
        std::shared_lock lock(mutex_);
        return pool_.size();
    }
    void getStats(CString tag){
        constant_map_.getCollisionStats(tag);
    }
private:
    struct Item{
        int64_t type {kType_NONE};
        size_t index {0};
        int hash;
        Item(){}
        Item(int64_t type, int hash):type(type),hash(hash){}
    };
    struct ItemHash{
        size_t operator()(const Item& info)const{
            return info.hash;
        }
    };
    struct ItemEQ{
        size_t operator()(const Item& i1, const Item& i2)const{
            return i1.hash == i2.hash && i1.type == i2.type;
        }
    };
    std::vector<Value> pool_;
    HighPerformanceHashTable<Item, int, ItemHash, ItemEQ> constant_map_;//rm repeat
    mutable std::shared_mutex mutex_;
};

class ConstantPool{
public:
    ConstantPool():ConstantPool(16, 0.75f){}
    ConstantPool(size_t initial_capacity, double max_load)
        :base_pool_(initial_capacity, max_load),
        obj_pool_(initial_capacity, max_load),
        string_pool_(initial_capacity, max_load)
    {}

    size_t addBool(bool val){
        return addBase(Value(val));
    }
    size_t addChar(char val){
        return addBase(Value(val));
    }
    size_t addShort(short val){
        return addBase(Value(val));
    }
    size_t addInt(int val){
        return addBase(Value(val));
    }
    size_t addLong(Long val){
        return addBase(Value(val));
    }
    size_t addFloat(float val){
        return addBase(Value(val));
    }
    size_t addDouble(double val){
        return addBase(Value(val));
    }
    size_t addObject(const Value& val){
        return addObject0(val);
    }
    size_t addString(CString str){
        char* sptr = nullptr;
        {
            std::unique_lock lock(mutex_);
            sptr = (char*)string_pool_.intern(str);
        }
        return addObject0(Value(kType_STRING, new StringRef(sptr, 0, str.length())));
    }

    Value& getBaseAt(size_t index){
        return base_pool_.getAt(index);
    }
    const Value& getBaseAt(size_t index)const{
        return base_pool_.getAt(index);
    }
    int getBaseCount()const{
        return base_pool_.size();
    }

    Value& getObjectAt(size_t index){
        return obj_pool_.getAt(index);
    }
    const Value& getObjectAt(size_t index)const{
        return obj_pool_.getAt(index);
    }
    int getObjectCount()const{
        return obj_pool_.size();
    }

    size_t getConstantCount() const {
        return base_pool_.size() + obj_pool_.size();
    }
    void getStats(){
        std::cout << "\n=== 常量池统计 ===" << std::endl;
        std::cout << "总常量数: " << getConstantCount() << std::endl;

        base_pool_.getStats("primitive");
        obj_pool_.getStats("object");
        string_pool_.getStats();
    }

private:
    size_t addBase(const Value& val){
        return base_pool_.add(val);
    }
    size_t addObject0(const Value& val){
        return obj_pool_.add(val);
    }
    char* addToStringPool(CString str){
        std::unique_lock lock(mutex_);
        return (char*)string_pool_.intern(str);
    }

private:
    friend class ConstantPoolPlan;
    TypePool base_pool_;
    TypePool obj_pool_;
    StringInternPool string_pool_;
    mutable std::shared_mutex mutex_;
};

//-------------
class ConstantPoolPlan{
public:
    size_t* addBool(bool val){
        return &base_plans_.emplace_back(val).index;
    }
    size_t* addChar(char val){
        return &base_plans_.emplace_back(val).index;
    }
    size_t* addShort(short val){
        return &base_plans_.emplace_back(val).index;
    }
    size_t* addInt(int val){
        return &base_plans_.emplace_back(val).index;
    }
    size_t* addLong(Long val){
        return &base_plans_.emplace_back(val).index;
    }
    size_t* addFloat(float val){
        return &base_plans_.emplace_back(val).index;
    }
    size_t* addDouble(double val){
        return &base_plans_.emplace_back(val).index;
    }
    size_t* addObject(const Value& val){
        return &obj_plans_.emplace_back(val).index;
    }
    size_t* addObject(CString str){
        auto sptr = pool_.addToStringPool(str);
        return &obj_plans_.emplace_back(Value(kType_STRING,
                          new StringRef(sptr, 0, str.length()))).index;
    }
    Value& getAt(size_t index){
        auto& basePool = pool_.base_pool_;
        if(index < basePool.size()){
            return basePool.getAt(index);
        }
        index -= basePool.size();
        return pool_.obj_pool_.getAt(index);
    }
    const Value& getAt(size_t index)const{
        auto& basePool = pool_.base_pool_;
        if(index < basePool.size()){
            return basePool.getAt(index);
        }
        index -= basePool.size();
        return pool_.obj_pool_.getAt(index);
    }
    size_t size()const{
        return pool_.base_pool_.size() + pool_.obj_pool_.size();
    }
    void execute(){
        for(auto& p : base_plans_){
            p.index = pool_.base_pool_.add(p.val);
        }
        const auto baseSize = pool_.base_pool_.size();
        for(auto& p : obj_plans_){
            p.index = baseSize + pool_.obj_pool_.add(p.val);
        }
        base_plans_.clear();
        obj_plans_.clear();
    }

private:
    struct Plan{
        Value val;
        size_t index {(size_t)-1};
        Plan(const Value& val):val(val){}
    };
    ConstantPool pool_;
    List<Plan> base_plans_;
    List<Plan> obj_plans_;
};

}}

