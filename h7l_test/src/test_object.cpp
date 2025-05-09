#define CATCH_CONFIG_MAIN
#include "third/src/catch.hpp"

#include "h7/src/lang/Object.h"
#include "h7/src/lang/Class.h"
#include "h7/src/lang/Scope.h"
#include "h7/src/types/IntType.h"

using namespace h7l;

TEST_CASE( "Test_Object", "" ) {
    auto gc = std::make_unique<GlobalContext>();
    auto scope = std::make_unique<Scope>(gc.get());
    auto cls = std::make_unique<Class>("h7l_test_Class");
    cls->init(gc.get(), {MemberInfo(gc->getType(TYPE_ID_INT))}, {"a"});
    auto obj = scope->newObject(cls.get());
}
