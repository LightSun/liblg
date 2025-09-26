#pragma once

#include "runtime/impl/BaseTypeDelegates.h"
#include "runtime/context.h"

namespace h7l { namespace runtime {

class ClosureTypeDelegate: public ITypeDelegate{

public:
    DEF_TD_DISABLE_FUNC(add);
    DEF_TD_DISABLE_FUNC(sub);
    DEF_TD_DISABLE_FUNC(mul);
    DEF_TD_DISABLE_FUNC(div);
    DEF_TD_DISABLE_FUNC(lessThan);
    DEF_TD_DISABLE_FUNC(lessEquals);
    DEF_TD_DISABLE_FUNC(concat);
};


}}
