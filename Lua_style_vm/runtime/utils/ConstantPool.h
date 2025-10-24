#pragma once

#include "runtime/utils/StringInternPool.h"

namespace h7l { namespace runtime {

enum class ConstantType {
    NIL,
    BOOLEAN,
    INTEGER,
    DOUBLE,
    STRING,
    TABLE,
    FUNCTION
};

union ConstantValue {
    bool boolean;
    int64_t integer;
    double number;
    struct {
        const char* data;
        size_t length;
        uint32_t hash;
    } string;

    ConstantValue() : integer(0) {}
};


class ConstantObject {
public:
    virtual ~ConstantObject() = default;
    virtual ConstantType getType() const = 0;
    virtual size_t getMemoryUsage() const = 0;
    virtual bool equals(const ConstantObject* other) const = 0;
    virtual uint64_t hash() const = 0;
};

class StringConstant : public ConstantObject {
private:
    const char* data_;
    size_t length_;
    uint32_t hash_;

public:
    StringConstant(const char* data, size_t length, uint32_t hash)
        : data_(data), length_(length), hash_(hash) {}

    ConstantType getType() const override { return ConstantType::STRING; }

    size_t getMemoryUsage() const override {
        return sizeof(StringConstant) + length_ + 1;
    }

    bool equals(const ConstantObject* other) const override {
        if (other->getType() != ConstantType::STRING) return false;
        const StringConstant* str_other = static_cast<const StringConstant*>(other);
        return length_ == str_other->length_ &&
               std::memcmp(data_, str_other->data_, length_) == 0;
    }

    uint64_t hash() const override {
        return (static_cast<uint64_t>(hash_) << 32) | length_;
    }

    const char* getData() const { return data_; }
    size_t getLength() const { return length_; }
};

class ConstantPool {
private:
    struct Item{
        size_t index {0};
        uint64_t hash;
    };
    struct ItemHash{
        size_t operator()(const Item& info)const{
            return info.hash;
        }
    };
    struct ItemEQ{
        size_t operator()(const Item& i1, const Item& i2)const{
            return i1.hash == i2.hash && i1.index == i2.index;
        }
    };

    // 常量存储
    std::vector<ConstantValue> simple_constants_;
    std::vector<std::shared_ptr<ConstantObject>> complex_constants_;

    // 改进的字符串驻留池
    StringInternPool string_pool_;

    // 改进的哈希表用于常量去重
    HighPerformanceHashTable<Item, ItemHash, ItemEQ> constant_map_;

    // 内存使用统计
    std::atomic<size_t> memory_usage_{0};

    // 线程安全
    mutable std::shared_mutex mutex_;

    // 添加简单常量
    size_t addSimpleConstant(const ConstantValue& value, ConstantType type) {
        std::unique_lock lock(mutex_);

        uint64_t hash = computeSimpleHash(value, type);
        Item kitem;
        kitem.hash = hash;

        // 使用改进的哈希表查找
        auto exist_val = constant_map_.find(kitem);
        if (exist_val) {
            // 找到相同哈希的条目，需要验证是否真的相同
            if (exist_val->index < simple_constants_.size() &&
                compareSimpleConstants(simple_constants_[exist_val->index], value, type)) {
                return exist_val->index;
            }
            // 哈希冲突！需要处理
            std::cout << "简单常量哈希冲突！类型: " << static_cast<int>(type)
                      << ", 哈希: " << hash << std::endl;
        }
        kitem.index = simple_constants_.size();
        // add new
        simple_constants_.push_back(value);
        constant_map_.insert(kitem);

        memory_usage_.fetch_add(sizeof(ConstantValue), std::memory_order_relaxed);

        return kitem.index;
    }

    // 添加复杂常量
    size_t addComplexConstant(std::shared_ptr<ConstantObject> constant) {
        std::unique_lock lock(mutex_);

        uint64_t hash = constant->hash();
        Item kitem;
        kitem.hash = hash;

        // 查找现有常量
        auto exist_val = constant_map_.find(kitem);
        if (exist_val) {
            if (exist_val->index >= simple_constants_.size()) {
                size_t complex_index = exist_val->index - simple_constants_.size();
                if (complex_index < complex_constants_.size() &&
                    complex_constants_[complex_index]->equals(constant.get())) {
                    return exist_val->index;
                }
            }

            // 哈希冲突
            std::cout << "复杂常量哈希冲突！类型: " << static_cast<int>(constant->getType())
                      << ", 哈希: " << hash << std::endl;
        }

        // 添加新常量
        kitem.index = simple_constants_.size() + complex_constants_.size();
        complex_constants_.push_back(constant);
        constant_map_.insert(kitem);

        memory_usage_.fetch_add(constant->getMemoryUsage(), std::memory_order_relaxed);

        return kitem.index;
    }

    // 计算简单常量的哈希值
    uint64_t computeSimpleHash(const ConstantValue& value, ConstantType type) const {
        switch (type) {
        case ConstantType::NIL:
            return 0xDEADBEEF; // 特殊的魔法数
        case ConstantType::BOOLEAN:
            return value.boolean ? 0xBADF00D : 0xFACEFEED; // 不同的魔法数
        case ConstantType::INTEGER:
            // 使用更好的整数哈希
            {
                uint64_t x = static_cast<uint64_t>(value.integer);
                x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
                x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
                x = x ^ (x >> 31);
                return x;
            }

        case ConstantType::DOUBLE:{
            // 更好的双精度浮点数哈希
            static_assert(sizeof(double) == sizeof(uint64_t), "double must be 64 bits");
            uint64_t bits;
            std::memcpy(&bits, &value.number, sizeof(double));
            if (value.number == 0.0) bits = 0; // 标准化+0和-0
            bits = (bits ^ (bits >> 32)) * 0x45d9f3b3335b369ULL;
            bits = (bits ^ (bits >> 29)) * 0x45d9f3b3335b369ULL;
            bits = bits ^ (bits >> 32);
            return bits;
        }

        default:
            return 0;
        }
    }

    // 比较简单常量
    bool compareSimpleConstants(const ConstantValue& a, const ConstantValue& b, ConstantType type) const {
        switch (type) {
        case ConstantType::NIL:
            return true;
        case ConstantType::BOOLEAN:
            return a.boolean == b.boolean;
        case ConstantType::INTEGER:
            return a.integer == b.integer;
        case ConstantType::DOUBLE:
            // 处理浮点数精度问题
            return std::abs(a.number - b.number) < 1e-15;
        default:
            return false;
        }
    }

public:
    ConstantPool() : constant_map_(16, 0.75) {
        simple_constants_.reserve(64);
        complex_constants_.reserve(32);
    }

    // 添加各种常量类型的方法
    size_t addNil() {
        ConstantValue value;
        return addSimpleConstant(value, ConstantType::NIL);
    }

    size_t addBoolean(bool b) {
        ConstantValue value;
        value.boolean = b;
        return addSimpleConstant(value, ConstantType::BOOLEAN);
    }

    size_t addInteger(int64_t i) {
        ConstantValue value;
        value.integer = i;
        return addSimpleConstant(value, ConstantType::INTEGER);
    }

    size_t addDouble(double d) {
        ConstantValue value;
        value.number = d;
        return addSimpleConstant(value, ConstantType::DOUBLE);
    }

    size_t addString(const char* data, size_t length) {
        const char* interned_data = string_pool_.intern(data, length);
        uint32_t hash = 0;

        const uint32_t prime = 16777619u;
        hash = 2166136261u;
        for (size_t i = 0; i < length; ++i) {
            hash ^= static_cast<uint32_t>(interned_data[i]);
            hash *= prime;
        }

        auto string_constant = std::make_shared<StringConstant>(interned_data, length, hash);
        return addComplexConstant(string_constant);
    }

    size_t addString(const char* cstr) {
        return addString(cstr, std::strlen(cstr));
    }

    size_t addString(const std::string& str) {
        return addString(str.data(), str.length());
    }

    // 获取常量
    ConstantValue getSimpleConstant(size_t index) const {
        std::shared_lock lock(mutex_);
        if (index < simple_constants_.size()) {
            return simple_constants_[index];
        }
        return ConstantValue();
    }

    std::shared_ptr<ConstantObject> getComplexConstant(size_t index) const {
        std::shared_lock lock(mutex_);
        if (index >= simple_constants_.size()) {
            size_t complex_index = index - simple_constants_.size();
            if (complex_index < complex_constants_.size()) {
                return complex_constants_[complex_index];
            }
        }
        return nullptr;
    }

    const char* getStringData(size_t index) const {
        auto constant = getComplexConstant(index);
        if (auto string_constant = std::dynamic_pointer_cast<StringConstant>(constant)) {
            return string_constant->getData();
        }
        return nullptr;
    }

    // 统计信息
    size_t getMemoryUsage() const {
        return memory_usage_.load(std::memory_order_relaxed);
    }

    size_t getConstantCount() const {
        std::shared_lock lock(mutex_);
        return simple_constants_.size() + complex_constants_.size();
    }

    void getStats(){
        std::cout << "\n=== 常量池统计 ===" << std::endl;
        std::cout << "总常量数: " << getConstantCount() << std::endl;
        std::cout << "内存使用: " << getMemoryUsage() << " 字节" << std::endl;

        constant_map_.getCollisionStats();
        string_pool_.getStats();
    }
};
//------------------------------


}}

