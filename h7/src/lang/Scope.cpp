#include "Scope.h"
#include "src/lang/Class.h"
#include "src/lang/Object.h"

using namespace h7l;


Scope::~Scope(){

}
Object* Scope::newObject(CString objName, Class* base, CList<int> shapes){
    auto it = m_objMap.emplace(objName, std::make_unique<Object>(this, base, shapes));
    if(it.second){
        return it.first->second.get();
    }
    return nullptr;
}
