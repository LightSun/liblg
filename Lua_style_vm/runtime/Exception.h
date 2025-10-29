#pragma once

#include "runtime/IObjectType.h"
#include "runtime/utils/privates.h"

namespace h7l { namespace runtime {

struct Exception: public BaseObjectType<Exception>{

    int code {0};
    int opcode {0};
    String msg;

    Exception(){}
    Exception(int code,CString msg):code(code),msg(msg) {}

    static Exception* make(int opcode, CString msg){
        Exception* ex = new Exception();
        ex->opcode = opcode;
        ex->msg = msg;
        return ex;
    }

    void reset(){
        code = -1;
        opcode = -1;
        msg = "";
    }
    void printTo(std::stringstream& ss)override{
        ss << "code = " << code << ", ";
        ss << "opcode = " << opcode << ", ";
        ss << "msg = " << msg;
    }
    bool equals(IObjectType* obj) const override{
        auto oth = dynamic_cast<Exception*>(obj);
        if(oth == nullptr) return false;
        return code == oth->code
               && opcode == oth->opcode
               && msg == oth->msg;
    }
    int hashCode()const override{
        int var = 0;
        var = var * 31 + code;
        var = var * 31 + opcode;
        var = var * 31 + str_hash(msg.data(), msg.length());
        return var;
    }
};


}}
