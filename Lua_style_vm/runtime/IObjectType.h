#pragma once

#include <atomic>
#include <string>
#include <sstream>
#include <vector>
#include "runtime/common_base.h"

namespace h7l { namespace runtime {

#ifdef _WIN32
    typedef long long Long;
typedef unsigned long long ULong;
#else
    typedef long int Long;
typedef unsigned long int ULong;
#endif

using String = std::string;
using CString = const std::string&;
template<typename T>
using List = std::vector<T>;

template<typename T>
using CList = const std::vector<T>&;

enum CompareResult{
    kCmpRet_ERROR_ARG    = -4,   //arg error
    kCmpRet_ERROR_CANT_CMP = -3, //like bool can't cmp to non-bool, primitive cmp to object, t1 and t2 has no extend
    kCmpRet_NAN  = -2,           //has NAN
    kCmpRet_LESS = -1,
    kCmpRet_EQUALS,
    kCmpRet_GREATER
};

enum Type{
    kType_NONE = -1,
    kType_S8,
    kType_U8,
    kType_S16,
    kType_U16,
    kType_S32,
    kType_U32,
    kType_S64,
    kType_U64,
    kType_BOOL,
    kType_FLOAT,
    kType_DOUBLE,
    kType_VOID,
    kType_NULL,
    kType_TABLE,
    kType_ARRAY,
    kType_CLOSURE,
    kType_STRING,
    kType_CFUNCTION,
    kType_EXCEPTION,
};

//StringRef
//struct Table
//struct Closure
//string CFunction
struct IObjectType;

typedef union TypeValue{
    char s8;
    unsigned char u8;
    short s16;
    unsigned short u16;
    int s32;
    unsigned int u32;
    Long s64;
    ULong u64;
    bool b;
    float f32;
    double f64;
    IObjectType* ptr;
}TypeValue;

struct IObjectType
{
    std::atomic_int refCnt {1};

    virtual ~IObjectType(){
        //unref();
    }

    void ref(){
        refCnt.fetch_add(1, std::memory_order_relaxed);
    }
    void unref(){
        if(refCnt.fetch_add(-1, std::memory_order_acq_rel) == 1){
            delete this;
        }
    }
    void print(){
        std::stringstream ss;
        printTo(ss);
        std::cout << ss.str() << std::endl;
    }
    String toString(){
        std::stringstream ss;
        printTo(ss);
        return ss.str();
    }
    virtual void printTo(std::stringstream& ss) = 0;

    virtual bool equals(IObjectType*) const = 0;

    virtual int hashCode() const = 0;
};

template<typename T>
struct BaseObjectType: public IObjectType{

    friend std::ostream& operator<<(std::ostream& os, const T& bni){
        auto& b = ((T&)bni);
        std::stringstream ss;
        b.printTo(ss);
        os << ss.str();
        return os;
    }
};

//---------
//---------

template <typename T> static inline T* SkRef(T* obj) {
    MED_ASSERT(obj);
    obj->ref();
    return obj;
}
template <typename T> static inline T* SkSafeRef(T* obj) {
    if (obj) {
        obj->ref();
    }
    return obj;
}
template <typename T> static inline void SkSafeUnref(T* obj) {
    if (obj) {
        obj->unref();
    }
}
template <typename T> class sk_sp {
public:
    using element_type = T;

    constexpr sk_sp() : fPtr(nullptr) {}
    constexpr sk_sp(std::nullptr_t) : fPtr(nullptr) {}

    sk_sp(const sk_sp<T>& that) : fPtr(SkSafeRef(that.get())) {}
    template <typename U,
             typename = typename std::enable_if<std::is_convertible<U*, T*>::value>::type>
    sk_sp(const sk_sp<U>& that) : fPtr(SkSafeRef(that.get())) {}

    sk_sp(sk_sp<T>&& that) : fPtr(that.release()) {}
    template <typename U,
             typename = typename std::enable_if<std::is_convertible<U*, T*>::value>::type>
    sk_sp(sk_sp<U>&& that) : fPtr(that.release()) {}

    explicit sk_sp(T* obj) : fPtr(obj) {}

    ~sk_sp() {
        SkSafeUnref(fPtr);
        fPtr = nullptr;
    }

    sk_sp<T>& operator=(std::nullptr_t) { this->reset(); return *this; }

    sk_sp<T>& operator=(const sk_sp<T>& that) {
        if (this != &that) {
            this->reset(SkSafeRef(that.get()));
        }
        return *this;
    }
    template <typename U,
             typename = typename std::enable_if<std::is_convertible<U*, T*>::value>::type>
    sk_sp<T>& operator=(const sk_sp<U>& that) {
        this->reset(SkSafeRef(that.get()));
        return *this;
    }

    sk_sp<T>& operator=(sk_sp<T>&& that) {
        this->reset(that.release());
        return *this;
    }
    template <typename U,
             typename = typename std::enable_if<std::is_convertible<U*, T*>::value>::type>
    sk_sp<T>& operator=(sk_sp<U>&& that) {
        this->reset(that.release());
        return *this;
    }

    T& operator*() const {
        MED_ASSERT(this->get() != nullptr);
        return *this->get();
    }

    explicit operator bool() const { return this->get() != nullptr; }

    T* get() const { return fPtr; }
    T* operator->() const { return fPtr; }

    void reset(T* ptr = nullptr) {
        // Calling fPtr->unref() may call this->~() or this->reset(T*).
        // http://wg21.cmeerw.net/lwg/issue998
        // http://wg21.cmeerw.net/lwg/issue2262
        T* oldPtr = fPtr;
        fPtr = ptr;
        SkSafeUnref(oldPtr);
    }

    T* release() {
        T* ptr = fPtr;
        fPtr = nullptr;
        return ptr;
    }

    void swap(sk_sp<T>& that) /*noexcept*/ {
        std::swap(fPtr, that.fPtr);
    }

private:
    T*  fPtr;
};

template <typename T> inline void swap(sk_sp<T>& a, sk_sp<T>& b) /*noexcept*/ {
    a.swap(b);
}

template <typename T, typename U> inline bool operator==(const sk_sp<T>& a, const sk_sp<U>& b) {
    return a.get() == b.get();
}
template <typename T> inline bool operator==(const sk_sp<T>& a, std::nullptr_t) /*noexcept*/ {
    return !a;
}
template <typename T> inline bool operator==(std::nullptr_t, const sk_sp<T>& b) /*noexcept*/ {
    return !b;
}

template <typename T, typename U> inline bool operator!=(const sk_sp<T>& a, const sk_sp<U>& b) {
    return a.get() != b.get();
}
template <typename T> inline bool operator!=(const sk_sp<T>& a, std::nullptr_t) /*noexcept*/ {
    return static_cast<bool>(a);
}
template <typename T> inline bool operator!=(std::nullptr_t, const sk_sp<T>& b) /*noexcept*/ {
    return static_cast<bool>(b);
}

template <typename C, typename CT, typename T>
auto operator<<(std::basic_ostream<C, CT>& os, const sk_sp<T>& sp) -> decltype(os << sp.get()) {
    return os << sp.get();
}

template <typename T, typename... Args>
sk_sp<T> sk_make_sp(Args&&... args) {
    return sk_sp<T>(new T(std::forward<Args>(args)...));
}

template <typename T> sk_sp<T> sk_ref_sp(T* obj) {
    return sk_sp<T>(SkSafeRef(obj));
}

template <typename T> sk_sp<T> sk_ref_sp(const T* obj) {
    return sk_sp<T>(const_cast<T*>(SkSafeRef(obj)));
}

}}
