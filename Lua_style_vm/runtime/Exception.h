#pragma once

#include "runtime/IObjectType.h"
#include "runtime/utils/privates.h"

namespace h7l { namespace runtime {

struct Exception: public BaseObjectType<Exception>{

    int code {-1};
    String msg;

    Exception(){}
    Exception(int code,CString msg):code(code),msg(msg) {}

    void reset(){
        code = -1;
        msg = "";
    }
    void printTo(std::stringstream& ss)override{
        ss << "code = " << code << ", ";
        ss << "msg = " << msg;
    }
    bool equals(IObjectType* obj) const override{
        auto oth = dynamic_cast<Exception*>(obj);
        if(oth == nullptr) return false;
        return code == oth->code && msg == oth->msg;
    }
    int hashCode()const override{
        return code * 31 + str_hash(msg.data(), msg.length());
    }
};


}}
