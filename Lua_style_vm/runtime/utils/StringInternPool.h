#pragma once

#include "runtime/utils/HighPerformanceHashTable.h"

namespace h7l { namespace runtime {

using String = std::string;
using CString = const std::string&;

class StringInternPool {
private:
    struct StringInfoHash{
        size_t operator()(const String& info)const{
            return computeHash(info.data(), info.length());
        }
        // FNV-1a哈希算法
        static uint32_t computeHash(const char* data, size_t length) {
            const uint32_t prime = 16777619u;
            uint32_t hash = 2166136261u;

            for (size_t i = 0; i < length; ++i) {
                hash ^= static_cast<uint32_t>(data[i]);
                hash *= prime;
            }
            return hash;
        }
        static inline int str_hash(const char* data, size_t len){
            int var1 = 0;
            if (len > 0) {
                for(size_t i = 0 ; i < len ; ++i){
                    var1 = 31 * var1 + data[i];
                }
            }
            return var1;
        }
    };
    using STable = HighPerformanceHashTable<String, uint32_t, StringInfoHash>;

    STable hash_table_;

    // 双重哈希检查
    static inline bool compareStrings(const char* a, const char* b, size_t length){
        // quick head-8
        if (length >= 8) {
            uint64_t* a64 = (uint64_t*)a;
            uint64_t* b64 = (uint64_t*)b;
            if (*a64 != *b64) return false;
        }
        return std::memcmp(a, b, length) == 0;
    }

public:
    StringInternPool(size_t initial_capacity, double max_load):
        hash_table_(initial_capacity, max_load){}

    StringInternPool():StringInternPool(16, 0.75f){}

    ~StringInternPool() {
    }

    // 获取或插入字符串
    const char* intern(const char* data, size_t length) {
        String str = data ? String(data, length) : "";

        // 查找现有字符串
        auto existing = hash_table_.find(str);
        if (existing) {
            // 哈希匹配，但需要验证内容是否真的相同
            if (existing->length() == length && compareStrings(existing->data(), data, length)) {
                return existing->data();
            }
            // 哈希冲突！使用更精确的比较
            // 这里我们线性搜索所有具有相同哈希的字符串
            // 在实际实现中，这应该很少发生
            std::cout << "警告：检测到哈希冲突！哈希: " << StringInfoHash::computeHash(data, length)
                      << ", 长度: " << length << std::endl;
        }
        return hash_table_.insert(str)->data();
    }

    const char* intern(const char* cstr) {
        return intern(cstr, std::strlen(cstr));
    }

    const char* intern(const std::string& str) {
        return intern(str.data(), str.length());
    }

    // 获取统计信息
    void getStats(){
        hash_table_.getCollisionStats("string");
    }
};

}}
