#define CATCH_CONFIG_MAIN
#include "third/src/catch.hpp"

#include "h7/src/types/IntType.h"
#include "h7l_test/src/test_common.h"

using namespace h7l;

TEST_CASE( "Test_Object", "" ) {
//    auto gc = std::make_unique<GlobalContext>();
//    auto scope = std::make_unique<Scope>(gc.get());
//    auto cls = std::make_unique<Class>("h7l_test_Class");
//    cls->init(gc.get(), {MemberInfo(gc->getType(TYPE_ID_INT))}, {"a"});
    //auto obj = scope->newObject(cls.get());
}

TEST_CASE("Test_Object_pri_char") {
    TestContext contxt;
    auto& obj = contxt.getObj();;
    char val = 99;
    obj.wrapOfPrimitive(val);
    REQUIRE(obj.mb.getChar(0) == val);
    obj.castPrimitive(kPriType_INT);
    //printf("obj.mb.getChar(0) = %d\n", obj.mb.getChar(0));
    REQUIRE(obj.mb.getChar(0) == 0);
    REQUIRE(obj.mb.getInt(0) == val);
    REQUIRE(obj.mb.getUInt(0) == val);

    obj.castPrimitive(kPriType_FLOAT);
    //printf("obj.mb.getFloat(0) = %.3f\n", obj.mb.getFloat(0));
    REQUIRE(obj.mb.getFloat(0) == (float)val);

    double dv = 0;
    obj.castPrimitiveTo(kPriType_DOUBLE, &dv);
    auto diff = std::abs(dv - val);
    REQUIRE(diff <= 0.000001);
    obj.castPrimitive(kPriType_SHORT);
    REQUIRE(obj.mb.getShort(0) == val);
    obj.castPrimitive(kPriType_LONG);
    REQUIRE(obj.mb.getLong(0) == val);
    obj.castPrimitive(kPriType_BOOL);
    REQUIRE(obj.mb.getBool(false) == true);
    //-------------------
    obj.reset();
    //
    obj.wrapOfPrimitivePtr(&dv);
    obj.castPrimitive(kPriType_LONG);
    REQUIRE(obj.mb.getLong(0) == val);
}
