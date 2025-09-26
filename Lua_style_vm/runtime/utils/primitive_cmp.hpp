#pragma once

#include <type_traits>
#include <cmath>
#include "runtime/IObjectType.h"

namespace h7l {  namespace runtime {

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


}}
