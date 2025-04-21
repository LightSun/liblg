#define CATCH_CONFIG_MAIN
#include "catch_test.h"
#include "h7/src/lang/Object.h"

TEST_CASE( "Test Object", "" ) {
    REQUIRE( factorial( 1) == 1 );
    REQUIRE( factorial( 2) == 2 );
    REQUIRE( factorial( 3) == 6 );
    REQUIRE( factorial(10) == 3'628'800 );
}
