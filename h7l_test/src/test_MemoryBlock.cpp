#include "third/src/catch.hpp"

//#define H7L_DBG_ShareData_DELETE 1
#include "src/lang/MemoryBlock.h"
#include "src/cons.h"

TEST_CASE("Test_MemoryBlock_int"){
    using namespace h7l;
    int val = 10086;
    MemoryBlock mb;
    mb.initWithWrapPrimitive(kPriType_INT, &val);
    REQUIRE(mb.getInt() == val);
    REQUIRE(mb.getUInt(10000) == 10086);
    REQUIRE(mb.getShort(10000) == 10000);
    mb.initWithWrapPrimitivePtr(kPriType_INT, &val);
    REQUIRE(mb.getDataPtr() == &val);
    REQUIRE(mb.getPrimitiveType() == kPriType_INT);
    mb.reset();
    REQUIRE(mb.getPrimitiveType() == kPriType_NONE);
    REQUIRE(!mb.getPrimitiveValue(&val));
}

TEST_CASE("Test_MemoryBlock_int_with_set"){
    using namespace h7l;
    int val = 10086;
    MemoryBlock mb;
    mb.initWithWrapPrimitive(kPriType_INT, &val);
    REQUIRE(mb.getInt() == val);
    mb.setPrimitiveValue(999.0);
    REQUIRE(mb.getInt() == 999);

    mb.setPrimitiveValue((short)128);
    REQUIRE(mb.getInt() == 128);
}

TEST_CASE("Test_MemoryBlock_char"){
    using namespace h7l;
    char val = 99;
    MemoryBlock mb;
    mb.initWithWrapPrimitive(kPriType_CHAR, &val);
    REQUIRE(mb.getChar() == val);
    mb.initWithWrapPrimitivePtr(kPriType_CHAR, &val);
    REQUIRE(mb.getDataPtr() == &val);
    REQUIRE(mb.getPrimitiveType() == kPriType_CHAR);
    mb.reset();
    REQUIRE(mb.getPrimitiveType() == kPriType_NONE);
    REQUIRE(!mb.getPrimitiveValue(&val));
}

TEST_CASE("Test_MemoryBlock_long"){
    using namespace h7l;
    long long val = 99;
    MemoryBlock mb;
    mb.initWithWrapPrimitive(kPriType_LONG, &val);
    REQUIRE(mb.getLong() == val);
    mb.initWithWrapPrimitivePtr(kPriType_LONG, &val);
    REQUIRE(mb.getDataPtr() == &val);
    REQUIRE(mb.getPrimitiveType() == kPriType_LONG);
    mb.reset();
    REQUIRE(mb.getPrimitiveType() == kPriType_NONE);
    REQUIRE(!mb.getPrimitiveValue(&val));
}

TEST_CASE("Test_MemoryBlock_float"){
    using namespace h7l;
    float val = 99;
    MemoryBlock mb;
    mb.initWithWrapPrimitive(kPriType_FLOAT, &val);
    REQUIRE(mb.getFloat() == val);
    mb.initWithWrapPrimitivePtr(kPriType_FLOAT, &val);
    REQUIRE(mb.getDataPtr() == &val);
    REQUIRE(mb.getPrimitiveType() == kPriType_FLOAT);
    mb.reset();
    REQUIRE(mb.getPrimitiveType() == kPriType_NONE);
    REQUIRE(!mb.getPrimitiveValue(&val));
}

TEST_CASE("Test_MemoryBlock_bool"){
    using namespace h7l;
    bool val = true;
    MemoryBlock mb;
    mb.initWithWrapPrimitive(kPriType_BOOL, &val);
    REQUIRE(mb.getBool() == val);
    mb.initWithWrapPrimitivePtr(kPriType_BOOL, &val);
    REQUIRE(mb.getDataPtr() == &val);
    REQUIRE(mb.getPrimitiveType() == kPriType_BOOL);
    mb.reset();
    REQUIRE(mb.getPrimitiveType() == kPriType_NONE);
    REQUIRE(!mb.getPrimitiveValue(&val));
}
//--------------------------------
TEST_CASE("Test_MemoryBlock_share"){
    using namespace h7l;
    auto sd = ShareData::New(1024);
    MemoryBlock mb;
    sd->ref();
    mb.initWithShareData(sd, 1022, 1024);
    REQUIRE(mb.hasFlag(kMemFlag_SHARE));
    sd->unref();
}

