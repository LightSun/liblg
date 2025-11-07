#define CATCH_CONFIG_MAIN

#include "third/src/catch.hpp"
#include "runtime/Array.h"

using namespace h7l::runtime;

namespace test {
struct ArrayTester{

    void test_simple(){
        auto arr = std::unique_ptr<Array>(Array::New(kType_S32, {10}));
        for(size_t i = 0 ; i < 10 ; ++i){
            Value val(i + 1);
            arr->setGlobalElement(i, &val);
            auto val2 = arr->getElement(i);
            REQUIRE(val == val2);
            auto val3 = arr->getGlobalElement(i);
            REQUIRE(val == val3);
        }
        arr->print();
        REQUIRE(arr->getBaseElementType() == kType_S32);
        REQUIRE(arr->toString() == "[1,2,3,4,5,6,7,8,9,10]");
    }
    void test_subArray2(){
        auto arr = std::unique_ptr<Array>(Array::New(kType_S32, {2,3}));
        auto cnt = arr->getBaseElementCount();
        for(size_t i = 0 ; i < cnt ; ++i){
            Value val(i + 1);
            arr->setGlobalElement(i, &val);
            auto val3 = arr->getGlobalElement(i);
            REQUIRE(val == val3);
        }
        arr->print();
        auto subArr1 = arr->getElement(0);
        auto subArr2 = arr->getElement(1);
        //subArr.print();
        REQUIRE(subArr1.toString() == "[1,2,3]");
        REQUIRE(subArr2.toString() == "[4,5,6]");
    }
    void test_subArray3(){
        auto arr = createArray({2,3,4}, 1);
        auto cnt = arr->getBaseElementCount();
        for(size_t i = 0 ; i < cnt ; ++i){
            Value val(i + 1);
            arr->setGlobalElement(i, &val);
            auto val3 = arr->getGlobalElement(i);
            REQUIRE(val == val3);
        }
        arr->print();
        auto subArr1 = arr->getElement(0);
        REQUIRE(subArr1.toString() == "[\n [1,2,3,4],\n [5,6,7,8],\n [9,10,11,12]\n]");
        auto subArr2 = arr->getElement(1);
        REQUIRE(subArr2.toString() == "[\n [13,14,15,16],\n [17,18,19,20],\n [21,22,23,24]\n]");
        auto ssub1 = subArr1.getPtr<Array>()->getElement(0);
        ssub1.print();
        //
        auto inArr = Array::New(kType_S32, {4});
        for(size_t i = 0 ; i < inArr->getBaseElementCount() ; ++i){
            Value val(i + 1000);
            inArr->setGlobalElement(i, &val);
        }
        //test set sub-arr
        auto inValue = Value::makeArray(inArr);
        String msg;
        if(!subArr1.getPtr<Array>()->setElement(0, &inValue, &msg)){
            fprintf(stderr, "setElement(0,xxx) failed. msg = %s.\n", msg.data());
        }else{
            ssub1.print();
        }
    }
    void testMerge(){
        auto arr1 = createArray({2,3,1}, 1);
        auto arr2 = createArray({2,3,2}, 100);
        auto info = arr1->computeDiffForMerge(arr2.get());
        if(info.shouldMerge()){
            auto arr3 = arr1->merge(arr2.get(), info.dimIndex);
            arr3.print();
        }
    }

private:
    std::unique_ptr<Array> createArray(CList<int> shapes, int firstEleVal){
        auto arr = std::unique_ptr<Array>(Array::New(kType_S32, shapes));
        auto cnt = arr->getBaseElementCount();
        for(size_t i = 0 ; i < cnt ; ++i){
            Value val(i + firstEleVal);
            arr->setGlobalElement(i, &val);
            auto val3 = arr->getGlobalElement(i);
            REQUIRE(val == val3);
        }
        return arr;
    }
};
}

TEST_CASE( "test_array", "[Array]" ) {
    test::ArrayTester tester;
    //tester.test_simple();
    //tester.test_subArray2();
    //tester.test_subArray3();
    tester.testMerge();
}
