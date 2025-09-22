#pragma once

#include <stack>
#include <vector>
#include "FunctionProto.h"

namespace h7l {

class VM
{
public:
    VM(): pc_(0), running_(false) {
        registers_.resize(20);
    }
    void execute(Scope* scope,std::shared_ptr<FunctionProto> func);

private:
    std::vector<Value> registers_;  // 寄存器数组
    std::stack<std::vector<Value>> callStack_;  // 调用栈，保存寄存器状态
    std::stack<int> returnPc_;      // 返回地址
    int pc_;                        // 程序计数器
    std::atomic_bool running_ {false};                  // 运行标志
    std::shared_ptr<FunctionProto> currentFunction_;
};


}

