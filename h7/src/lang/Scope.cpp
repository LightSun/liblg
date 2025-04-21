#include "Scope.h"
#include "src/lang/Class.h"
#include "src/lang/Object.h"

using namespace h7l;


Scope::~Scope(){

}
Object* Scope::newObject(Class* base){
    auto it = m_objs.insert(m_objs.end(), std::make_unique<Object>(
                                              this, base, nullptr));
    return it->get();
}
Object* Scope::newArray(Class* base, CList<int> shapes){
    auto it = m_objs.insert(m_objs.end(), std::make_unique<Object>(
                                              this, base, shapes));
    return it->get();
}
