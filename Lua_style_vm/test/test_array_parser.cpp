#include "third/src/catch.hpp"
#include "runtime/utils/ArrayParser.h"

using namespace h7l::runtime;

TEST_CASE( "test_ArrayParser", "[ArrayParser]" ) {
    String str0 = R"([
                         [
                          [1,2,3],
                          [4,5,6],
                          [7,8,9]
                         ],
                         [
                          [10,11,12],
                          [13,14,15],
                          [16,17,18]
                         ]
                  ])";
    auto str = SimpleArrayParser::removeBlank(str0);
    int dimC = SimpleArrayParser::getArrayDimCnt(str);
    List<int> shapes;
    List<int> eles;
    REQUIRE(SimpleArrayParser::parseInt(str, dimC, shapes, eles));
    REQUIRE(shapes[0] == 2);
    REQUIRE(shapes[1] == 3);
    REQUIRE(shapes[2] == 3);
    REQUIRE(eles.size() == 18);
    for(size_t i = 0 ;i < eles.size() ; ++i){
        REQUIRE(eles[i] == i + 1);
    }
}

TEST_CASE( "test_ArrayParser2", "[ArrayParser]" ) {
    String str0 = R"(
                            [
                             [
                              [1,2,3],
                              [4,5,6]
                             ],
                             [
                              [7,8,9],
                              [10,11,12]
                             ],
                             [
                              [13,14,15],
                              [16,17,18]
                             ]
                            ]
                        )";
    auto str = SimpleArrayParser::removeBlank(str0);
    int dimC = SimpleArrayParser::getArrayDimCnt(str);
    List<int> shapes;
    List<int> eles;
    REQUIRE(SimpleArrayParser::parseInt(str, dimC, shapes, eles));
    REQUIRE(shapes[0] == 3);
    REQUIRE(shapes[1] == 2);
    REQUIRE(shapes[2] == 3);
    REQUIRE(eles.size() == 18);
    for(size_t i = 0 ;i < eles.size() ; ++i){
        REQUIRE(eles[i] == i + 1);
    }
}
