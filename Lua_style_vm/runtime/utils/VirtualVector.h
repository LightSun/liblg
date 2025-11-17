#pragma once

#include <vector>

namespace h7l { namespace runtime {

template <typename T>
using List = std::vector<T>;

template<typename T>
class VirtualVector{
public:
    using CT = const T&;

    VirtualVector(int coreSize):m_coreSize(coreSize){
        m_vec.reserve(coreSize);
    }
    void resize(int s){
        m_vec.resize(s);
    }
    void sub(size_t delta){
        int target = std::max(m_vec.size() - delta, 0);
        int capTarget = std::max(target, m_coreSize);
        if(m_vec.capacity() < capTarget){
            m_vec.reserve(capTarget);
        }
        if(target >= 0){
            m_vec.resize(target);
        }
    }
    void add(size_t delta){
        int target = m_vec.size() + delta;
        int capTarget = std::max(target, m_coreSize);
        if(m_vec.capacity() < capTarget){
            m_vec.reserve(capTarget);
        }
        if(target >= 0){
            m_vec.resize(target);
        }
    }
    void push_back(CT t){
        m_vec.push_back(t);
    }
    T& at(int i){
        return m_vec.at(i);
    }
    auto begin(){
        return m_vec.begin();
    }
    auto begin()const{
        return m_vec.begin();
    }
    auto end(){
        return m_vec.end();
    }
    auto end()const{
        return m_vec.end();
    }
    T& operator[](int i){
        return m_vec[i];
    }

private:
    List<T> m_vec;
    int m_coreSize;
};


}}
