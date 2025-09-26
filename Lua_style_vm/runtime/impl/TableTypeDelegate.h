#pragma once

#include "runtime/impl/BaseTypeDelegates.h"
#include "runtime/IObjectType.h"
#include "runtime/Table.h"

namespace h7l { namespace runtime {

class TableTypeDelegate: public ITypeDelegate{

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
        auto ref1 = v1.getPtr<Table>();
        auto ref2 = v2.getPtr<Table>();
        return Value(kType_TABLE, (IObjectType*)ref1->merge(ref2));
    }
    Value sub(Value& v1, Value& v2) override{
        if(v2.isNullptr()){
            return v1;
        }
        auto ref1 = v1.getPtr<Table>();
        auto ref2 = v2.getPtr<Table>();
        std::unique_ptr<Table> ptr(ref1->copy());
        ptr->removeSame(ref2);
        return Value(kType_TABLE, (IObjectType*)ptr.release());
    }
};

}}
