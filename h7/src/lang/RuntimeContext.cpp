#include "src/lang/RuntimeContext.h"
#include "src/lang/Class.h"
#include "src/lang/Scope.h"
#include "src/VM.h"

using namespace h7l;

namespace h7l{
struct Class_VM : public Class{
    Class_VM(){
        id = H7L_CLASS_NAME_VM;
        structSize = sizeof(VM);
        arrayDimCnt = 0;
    }
};

struct Class_RuntimeContext : public Class{
    Class_RuntimeContext(){
        id = H7L_CLASS_NAME_RC;
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
std::unique_ptr<RuntimeContext> RuntimeContext::ofVM(VM* vm, Type* type){
    auto ptr = std::make_unique<RuntimeContext>(vm->scope_);
    ptr->type = type;
    return ptr;
}
