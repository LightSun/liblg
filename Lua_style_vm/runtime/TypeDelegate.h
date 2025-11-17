#pragma once

#include "runtime/Value.h"

namespace h7l { namespace runtime {

class ITypeDelegate{

public:

    virtual ~ITypeDelegate(){}

    virtual Value add(Value& v1, Value& v2) = 0;

    virtual Value sub(Value& v1, Value& v2) = 0;

    virtual Value mul(Value& v1, Value& v2) = 0;

    virtual Value div(Value& v1, Value& v2) = 0;

    virtual Value lessThan(Value& v1, Value& v2) = 0;

    virtual Value lessEquals(Value& v1, Value& v2) = 0;

};


}}
