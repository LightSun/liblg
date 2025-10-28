#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <map>
#include <cstring>
#include <type_traits>
#include <algorithm>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <functional>

namespace h7l { namespace runtime {

template<typename T,typename HT = uint64_t>
struct HashEntry {
    HT hash {0};
    T value;
    HashEntry* next {nullptr}; //link-list

    HashEntry(){}
    HashEntry(HT h, const T& v) : hash(h), value(v){}

    void delChildren(){
        HashEntry<T>* current = next;
        while (current) {
            HashEntry<T>* next = current->next;
            delete current;
            current = next;
        }
        next = nullptr;
    }
    void delAll(){
        delChildren();
        delete this;
    }
};

template<typename Value,
         typename HT = uint64_t,
         typename Hash = std::hash<Value>,
         typename Equal = std::equal_to<Value>
         >
class HighPerformanceHashTable {
private:
    using Entry = HashEntry<Value>;
    using EntryPtr = HashEntry<Value>*;
    std::vector<EntryPtr> table_;
    size_t size_;
    size_t capacity_;
    size_t mask_;
    float max_load_factor_;
    Hash hasher_;
    Equal equal_;
    std::shared_mutex mutex_;

    static inline int tableSizeFor(int var0) {
        int var1 = var0 - 1;
        var1 |= (var1 >> 1);
        var1 |= (var1 >> 2);
        var1 |= (var1 >> 4);
        var1 |= (var1 >> 8);
        var1 |= (var1 >> 16);
        return var1 < 0 ? 1 : (var1 >= 1073741824 ? 1073741824 : var1 + 1);
    }

    size_t computeIndex(HT hash) const {
        //return hash % capacity_;
        return hash & mask_;
    }
    void rehash() {
        auto old_table = std::move(table_);
        capacity_ = std::max(capacity_ * 2, size_t(16));
        table_.resize(capacity_);
        size_ = 0;

        for (auto& entry : old_table) {
            if (entry) {
                // 重新插入主条目
                insertInternal(entry->hash, entry->value);

                // 重新插入链地址法的冲突条目
                auto current = entry->next;
                while (current) {
                    insertInternal(current->hash, current->value);
                    HashEntry<Value>* next = current->next;
                    delete current;
                    current = next;
                }
            }
        }
    }

    Value* insertInternal(HT hash, const Value& value) {
        size_t index = computeIndex(hash);
        auto en = std::make_unique<Entry>(hash, value);

        if (!table_[index]) {
            // idle
            table_[index] = en.release();
            ++size_;
            return &table_[index]->value;
        }

        // pos has entry.
        if (table_[index]->hash == hash) {
            if(equal_(table_[index]->value, value)){
                return &table_[index]->value;
            }
        }

        // conflict .link-list
        auto current = table_[index];
        while (current->next) {
            current = current->next;
            if (isTheSameEntry(current, hash, value)){
                return &current->value;
            }
        }
        current->next = en.release();
        ++size_;
        return &current->next->value;
    }

    bool isTheSameEntry(HashEntry<Value>* en, uint64_t hash, const Value& value)const{
        if(en->hash == hash && equal_(en->value, value)){
            return true;
        }
        return false;
    }

public:
    HighPerformanceHashTable(size_t initial_capacity = 16, double max_load = 0.75)
        : size_(0), capacity_(tableSizeFor(initial_capacity)), max_load_factor_(max_load) {
        table_.resize(capacity_);
        mask_ = capacity_ - 1;
    }

    ~HighPerformanceHashTable() {
        for (auto& entry : table_) {
            if(entry){
                entry->delChildren();
                delete entry;
            }
        }
        table_.clear();
    }
    bool remove(const Value& value){
        std::unique_lock lock(mutex_);
        auto hash = hasher_(value);
        size_t index = computeIndex(hash);
        auto tab = table_[index];
        if (tab) {
            auto nextEntry = tab.next;
            //head
            if(isTheSameEntry(tab, hash, value)){
                delete tab;
                table_[index] = nextEntry;
                return true;
            }else{
                //other
                EntryPtr parent = tab;
                while (nextEntry) {
                    if(isTheSameEntry(nextEntry, hash, value)){
                        parent->next = nextEntry->next;
                        delete nextEntry;
                        return true;
                    }
                    parent = nextEntry;
                    nextEntry = nextEntry->next;
                }
            }
        }
        return false;
    }
    Value* insert(const Value& value) {
        std::unique_lock lock(mutex_);
        auto hash = hasher_(value);
        if (static_cast<float>(size_) / capacity_ > max_load_factor_) {
            rehash();
        }
        return insertInternal(hash, value);
    }

    Value* find(const Value& value) {
        std::shared_lock lock(mutex_);

        auto hash = hasher_(value);
        size_t index = computeIndex(hash);
        if (!table_[index]) {
            return nullptr;
        }
        // pos
        if (isTheSameEntry(table_[index], hash, value)) {
            return &table_[index]->value;
        }
        // conflict-list
        HashEntry<Value>* current = table_[index]->next;
        while (current) {
            if (isTheSameEntry(current, hash, value)) {
                return &current->value;
            }
            current = current->next;
        }
        return nullptr;
    }

    size_t size() const {
        std::shared_lock lock(mutex_);
        return size_;
    }
    size_t capacity() const {
        std::shared_lock lock(mutex_);
        return capacity_;
    }
    // count-stat
    void getCollisionStats(const std::string& tag = "")  {
        std::shared_lock lock(mutex_);

        size_t max_chain_length = 0;
        double avg_chain_length = 0;
        //
        size_t total_chains = 0;
        size_t chains_with_collisions = 0;
        std::map<int,int> lenMap;

        for (const auto& entry : table_) {
            if (entry) {
                total_chains++;
                size_t chain_length = 1;

                HashEntry<Value>* current = entry->next;
                while (current) {
                    chain_length++;
                    current = current->next;
                }

                if (chain_length > 1) {
                    chains_with_collisions++;
                    auto it = lenMap.find(chain_length);
                    if(it != lenMap.end()){
                        it->second++;
                    }else{
                        lenMap[chain_length] = 1;
                    }
                }else{
                    lenMap[1] ++;
                }

                max_chain_length = std::max(max_chain_length, chain_length);
            }
        }

        avg_chain_length = total_chains > 0 ? static_cast<double>(size_) / total_chains : 0.0;

        std::cout << tag << "哈希表统计:" << std::endl;
        std::cout << "  - 总条目数: " << size_ << std::endl;
        std::cout << "  - 容量: " << capacity_ << std::endl;
        std::cout << "  - 负载因子: " << (static_cast<double>(size_) / capacity_) << std::endl;
        std::cout << "  - 最大链长度: " << max_chain_length << std::endl;
        std::cout << "  - 平均链长度: " << avg_chain_length << std::endl;
        std::cout << "  - 有冲突的链数量: " << chains_with_collisions << std::endl;
        if(total_chains == 0){
            std::cout << "  - 冲突率: " << "0%" << std::endl;
        }else{
            std::cout << "  - 冲突率(冲突entry个数/total_chains): " <<
                (static_cast<double>(chains_with_collisions) / total_chains * 100)
                      << "%" << std::endl;
        }
        std::cout << "  - chain_len --> count " << std::endl;
        for(auto& [k,v] : lenMap){
            std::cout << " \t " << k << " -> " << v << std::endl;
        }
    }
};

}}
