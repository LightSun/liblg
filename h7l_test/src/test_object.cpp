#define CATCH_CONFIG_MAIN
#include "catch_test.h"

#include "h7/src/lang/Object.h"
#include "h7/src/lang/Class.h"
#include "h7/src/lang/Scope.h"
#include "h7/src/types/IntType.h"

using namespace h7l;

TEST_CASE( "Test Object", "" ) {
    auto gc = std::make_unique<GlobalContext>();
    auto scope = std::make_unique<Scope>(gc.get());
    auto cls = std::make_unique<Class>();
    cls->init(gc.get(), {MemberInfo(gc->getType(TYPE_ID_INT))}, {"a"});
    auto obj = scope->newObject(cls.get());
    obj->get
    REQUIRE( factorial(10) == 3'628'800 );
}
