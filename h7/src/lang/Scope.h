#pragma once

#include "src/lang/GlobalContext.h"

namespace h7l {

struct Object;
struct Class;

class Scope
{
public:
    Scope(GlobalContext* gc, Scope* parent = nullptr)
        :m_gc(gc),m_parent(parent){MED_ASSERT(gc);}
    ~Scope();

    void setName(CString name){
        m_name = name;
    }
    Scope* newChildScope(){
        int size = m_children.size();
        m_children.push_back(std::make_unique<Scope>(m_gc, this));
        return m_children[size].get();
    }
    Allocator* getAllocator(){
        return &m_gc->getAllocator();
    }

    Object* newObject(CString objName,Class* base, CList<int> shapes = {});

private:
    String m_name;
    GlobalContext* m_gc {nullptr};
    Scope* m_parent {nullptr};
    List<std::unique_ptr<Scope>> m_children;
    std::map<String, std::unique_ptr<Object>> m_objMap;
};

}
