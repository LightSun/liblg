#include "src/lang/RuntimeContext.h"
#include "src/lang/Class.h"
#include "src/lang/Scope.h"
#include "src/VM.h"

using namespace h7l;

#define CLASS_NAME_sRTC "h7l.RuntimeContext"
#define CLASS_NAME_VM "h7l.VM"

namespace h7l{
struct Class_VM : public Class{
    void init(){
        id = "h7l.VM";
        structSize = sizeof(VM);
        arrayDimCnt = 0;
    }
};

struct Class_RuntimeContext : public Class{
    Class_RuntimeContext(){
    }
    void init(){
        id = "h7l.RuntimeContext";
        //reg methods?
        structSize = sizeof(RuntimeContext);
        arrayDimCnt = 0;
    }
};
}
Class* RuntimeContext::CLASS = nullptr;
void RuntimeContext::init(GlobalContext* gc){
    CLASS = (Class*)gc->putRawType<Class_RuntimeContext>();
    gc->putRawType<Class_VM>();
}
RuntimeContext::RuntimeContext(Scope* scope): Object(scope, CLASS){
}
std::unique_ptr<RuntimeContext> ofVM(VM* vm, Type* type){
    auto ptr = std::make_unique<RuntimeContext>(vm->scope_);
    ptr->type = type;
    return ptr;
}
