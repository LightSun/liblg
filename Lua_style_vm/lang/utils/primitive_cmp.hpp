#pragma once

#include <type_traits>
#include <cmath>
#include "../cons.h"
#include "../common/c_common.h"

namespace h7l {

template <typename T>
inline int compare_with_epsilon(T a, T b, T epsilon) noexcept {
    if constexpr (std::is_floating_point_v<T>) {
        if (std::abs(a - b) <= epsilon) {
            return 0; // 在误差范围内视为相等
        }
    }
    return (a > b) - (a < b);
}

// 自动选择默认 epsilon（根据类型精度）
template <typename T>
inline int compare(T a, T b) noexcept {
    if constexpr (std::is_floating_point_v<T>) {
        if (std::isnan(a) || std::isnan(b)) {
            return kCmpRet_NAN;
        }
        T epsilon = std::numeric_limits<T>::epsilon() * 100; // 调整倍数
        return compare_with_epsilon(a, b, epsilon);
    } else {
        return (a > b) - (a < b);
    }
}

// 重载模板：处理不同类型的安全比较
template <typename T, typename U>
inline int compare(T a, U b) noexcept {
    using CommonType = typename std::common_type<T, U>::type;
    static_assert(std::is_arithmetic_v<T> && std::is_arithmetic_v<U>,
                  "Only arithmetic types are allowed.");

    if constexpr (std::is_floating_point_v<CommonType>) {
        if (std::isnan(a) || std::isnan(b)) {
            return kCmpRet_NAN;
        }
        // 浮点类型比较时启用 epsilon 检查
        CommonType epsilon = std::numeric_limits<CommonType>::epsilon() * 100;
        return compare_with_epsilon(
            static_cast<CommonType>(a),
            static_cast<CommonType>(b),
            epsilon
            );
    } else {
        // 整数类型保持原逻辑
        return compare(static_cast<CommonType>(a), static_cast<CommonType>(b));
    }
}

template <typename T, typename U>
inline int safe_compare(T a, U b) noexcept {
    if constexpr (std::is_signed_v<T> && std::is_unsigned_v<U>) {
        if (a < 0) return -1; // 负数直接判定为小
        return compare(static_cast<U>(a), b);
    } else if constexpr (std::is_unsigned_v<T> && std::is_signed_v<U>) {
        if (b < 0) return 1;  // 负数直接判定为大
        return compare(a, static_cast<T>(b));
    } else {
        return compare(a, b); // 其他情况走通用逻辑
    }
}

/*
#define _H7L_CMP_CURRENT()\
        auto& cur = asType();\
        if(cur < v){\
            return kCmpRet_LESS;\
        }else if(cur == v){\
            return kCmpRet_EQUALS;\
        }else{\
            return kCmpRet_GREATER;\
        }

#define _H7L_CMP_VALS()\
        if(cur < v){\
            return kCmpRet_LESS;\
        }else if(cur == v){\
            return kCmpRet_EQUALS;\
        }else{\
            return kCmpRet_GREATER;\
        }

#define _H7L_CMP_VALS_AUTO()\
        auto& cur = asType();\
        if(cur < v){\
            return kCmpRet_LESS;\
        }else if(cur == v){\
            return kCmpRet_EQUALS;\
        }else{\
            return kCmpRet_GREATER;\
        }

template<typename Derived, typename T>
class NumberComparator{
    const Derived &derived() const { return static_cast<const Derived &>(*this); }
public:
    void* ptr{nullptr};
    NumberComparator(void* ptr):ptr(ptr){}

    T& asType(){ return *(T*)ptr;}

    int compare(bool v){
        return derived().compare(v);
    }
    int compare(sint8 v){
        return derived().compare(v);
    }
    int compare(uint8 v){
        return derived().compare(v);
    }
    int compare(sint16 v){
        return derived().compare(v);
    }
    int compare(uint16 v){
        return derived().compare(v);
    }
    int compare(sint32 v){
        return derived().compare(v);
    }
    int compare(uint32 v){
        return derived().compare(v);
    }
    int compare(sint64 v){
        return derived().compare(v);
    }
    int compare(uint64 v){
        return derived().compare(v);
    }
};

class CharCmpImpl: public NumberComparator<CharCmpImpl, sint8>{
public:
    CharCmpImpl(void* ptr):NumberComparator(ptr){}

    int compare(bool){
        return kCmpRet_ERROR;
    }
    int compare(sint8 v){
        _H7L_CMP_CURRENT()
    }
    int compare(uint8 _v){
        int v = _v;
        _H7L_CMP_VALS_AUTO()
    }
    int compare(sint16 v){
        _H7L_CMP_CURRENT()
    }
    int compare(uint16 _v){
        int v = _v;
        _H7L_CMP_VALS_AUTO()
    }
    int compare(sint32 v){
        _H7L_CMP_CURRENT()
    }
    int compare(uint32 _v){
        sint64 v = _v;
        _H7L_CMP_VALS_AUTO()
    }
    int compare(sint64 v){
        _H7L_CMP_CURRENT()
    }
    int compare(uint64 _v){
        sint64 v = _v;
        _H7L_CMP_VALS_AUTO()
    }
};

class UCharCmpImpl : public NumberComparator<CharCmpImpl, uint8>{
public:
    UCharCmpImpl(void* ptr):NumberComparator(ptr){}

    int compare(bool){
        return kCmpRet_ERROR;
    }
    int compare(sint8 v){
        int cur = asType();
        _H7L_CMP_VALS()
    }
    int compare(uint8 v){
        _H7L_CMP_CURRENT()
    }
    int compare(sint16 v){
        _H7L_CMP_CURRENT()
    }
    int compare(uint16 v){
        _H7L_CMP_CURRENT()
    }
    int compare(sint32 v){
        _H7L_CMP_CURRENT()
    }
    int compare(uint32 v){
        _H7L_CMP_CURRENT()
    }
    int compare(sint64 v){
        _H7L_CMP_CURRENT()
    }
    int compare(uint64 v){
        _H7L_CMP_CURRENT()
    }
};
*/

}
