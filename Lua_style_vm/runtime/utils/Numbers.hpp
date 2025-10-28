#pragma once

#include <memory.h>
#include <inttypes.h>
#include <limits>
#include <type_traits>
#include <cmath>
#include <iostream>

namespace h7l { namespace runtime {

/**
min 是编译器认可的最小的正双精度浮点数；
eps 是可以保证 1.0 + eps != 1.0 这个表达式成立的最小的正双精度浮点数。
*/
// Test whether two float or double numbers are equal.
// ulp: units in the last place.
template <typename T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
IsAlmostEqual(T x, T y, int ulp = 2)
{
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return std::fabs(x - y) < std::numeric_limits<T>::epsilon() * std::fabs(x + y) * ulp
           // unless the result is subnormal
           || std::fabs(x - y) < std::numeric_limits<T>::min();
}

static inline void int_reverse_to(int val, const void* _dst){
    char* dst = (char*)_dst;
    dst[0] =  val & 0xff;
    dst[1] = (val & 0xff00) >> 8;
    dst[2] = (val & 0xff0000) >> 16;
    dst[3] = (val & 0xff000000) >> 24;
}

static inline void long_reverse_to(unsigned long long val, const void* _dst){
    char* dst = (char*)_dst;
    dst[0] = (val & 0xff);
    dst[1] = (val & 0xff00) >> 8;
    dst[2] = (val & 0xff0000) >> 16;
    dst[3] = (val & 0xff000000) >> 24;
    dst[4] = (val & 0xff00000000) >> 32;
    dst[5] = (val & 0xff0000000000) >> 40;
    dst[6] = (val & 0xff000000000000) >> 48;
    dst[7] = (val & 0xff00000000000000) >> 56;
}

static inline float intBitsToFloat(int i)
{
    union
    {
      int i;
      float f;
    } u;
    u.i = i;
    return u.f;
}


static inline int floatToRawIntBits(float f){
    union
    {
        int i;
        float f;
    } u;
    u.f = f;
    return u.i;
}

static inline long long doubleToLongBits(double value) {
    if(value != value){
        return 0x7ff8000000000000L;
    }
    long long longValue = 0;
    memcpy(&longValue, &value, sizeof(double));

    return longValue;
}

static inline long long doubleToRawLongBits(double value) {
    long long longValue = 0;
    memcpy(&longValue, &value, sizeof(double));
    return longValue;
}

static inline double longBitsToDouble(long long bits) {
    double doubleValue = 0;
    memcpy(&doubleValue, &bits, sizeof(int64_t));
    return doubleValue;
}

static inline bool isInfinite(double value) {
    return (value == std::numeric_limits<double>::infinity()
            || value == -std::numeric_limits<double>::infinity());
}

static inline bool isNaN(double value) {
    return (value != value);
}

static inline int64_t unsignedShiftLong(int64_t num, int64_t shift) {
    return (shift & 0x3f) == 0 ? num : (((uint64_t)num >> 1) & 0x7fffffffffffffffLL) >> ((shift & 0x3f) - 1);
}

static inline int32_t unsignedShiftInt(int32_t num, int32_t shift) {
    return (shift & 0x1f) == 0 ? num : (((uint32_t)num >> 1) & 0x7fffffff) >> ((shift & 0x1f) - 1);
}

// 8 to
static inline size_t roundUp8(size_t bytes) {
    return (((bytes) + sizeof(void*)-1) &~(sizeof(void*) - 1));
}

}}

