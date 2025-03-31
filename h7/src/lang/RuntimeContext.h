#pragma once

#include "src/lang/Object.h"

namespace h7l {

class Scope;
struct Class;
struct VM;
struct Type;
struct GlobalContext;

struct RuntimeContext: public Object{

    VM* vm {nullptr};
    Type* type {nullptr};

    static Class* CLASS;
    static void init(GlobalContext* gc);

    RuntimeContext(Scope* scope);

    static std::unique_ptr<RuntimeContext> ofVM(VM* vm, Type* type);
};
}
