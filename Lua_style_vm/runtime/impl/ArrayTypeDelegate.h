#pragma once

#include "runtime/impl/BaseTypeDelegates.h"
#include "runtime/IObjectType.h"
#include "runtime/Array.h"

namespace h7l { namespace runtime {

class ArrayTypeDelegate: public ITypeDelegate{

public:
    DEF_TD_DISABLE_FUNC(mul);
    DEF_TD_DISABLE_FUNC(div);
    DEF_TD_DISABLE_FUNC(lessThan);
    DEF_TD_DISABLE_FUNC(lessEquals);
    DEF_TD_DISABLE_FUNC(concat);

    Value add(Value& v1, Value& v2) override{
        if(v2.isNullptr()){
            return v1;
        }
        if(v1.isNullptr()){
            return v2;
        }
        auto ref1 = v1.getPtr<Array>();
        auto ref2 = v2.getPtr<Array>();

        auto info = ref1->computeDiffForMerge(ref2);
        if(info.shouldMerge()){
            return ref1->merge(ref2, info.dimIndex);
        }else{
            return Value(); //void means exception
        }
    }
    Value sub(Value& v1, Value& v2) override{
        if(v2.isNullptr()){
            return v1;
        }
        if(v1.isNullptr()){
            return Value::makeNull();
        }
        auto ref1 = v1.getPtr<Array>();
        auto ref2 = v2.getPtr<Array>();
        if(ref1->getShapes().size() == 1
            && ref2->getShapes().size() == 1
            && ref1->isElementComparable(ref2)){
            return ref1->removeSame(ref2);
        }
        //dims not support or can't cmp
        return Value();
    }
};

}}
