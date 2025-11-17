#pragma once

#include "runtime/impl/BaseTypeDelegates.h"

namespace h7l { namespace runtime {

class StringTypeDelegate: public ITypeDelegate{

public:
    DEF_TD_DISABLE_FUNC(sub);
    DEF_TD_DISABLE_FUNC(mul);
    DEF_TD_DISABLE_FUNC(div);
    DEF_TD_DISABLE_FUNC(lessThan);
    DEF_TD_DISABLE_FUNC(lessEquals);

    Value add(Value& v1, Value& v2) override{
        auto ref1 = v1.getPtr<StringRef>();
        auto ref2 = v2.getPtr<StringRef>();
        return Value::makeString(ref1->toString() + ref2->toString());
    }
};


}}
