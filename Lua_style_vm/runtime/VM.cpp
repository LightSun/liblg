#include "VM.h"

namespace h7l { namespace runtime {

struct ClosureDeleter{

    void operator()(Closure* p)const{
        if(p){
            p->unref();
        }
    }
};

void VM::execute(std::shared_ptr<FunctionProto> func){
    callStack_.emplace(new Closure(func), 0, 0, func->numRegisters);
    running = true;

    // 主执行循环
    while (running && !callStack_.empty()) {
        CallFrame& frame = callStack_.top();
        auto closure = frame.closure;
        pc = frame.pc;

        if (pc >= (int)closure->proto->instructions.size()) {
            fprintf(stderr, "pc >= instructions.size()\n");
            // 函数结束，关闭upvalues
            closeUpvaluesAbove(frame.base);
            callStack_.pop();
            if (!callStack_.empty()) {
                // 返回调用者
                CallFrame& caller = callStack_.top();
                caller.pc++; // 继续执行下一条指令
            }
            continue;
        }

        const Instruction& instr = closure->proto->instructions[pc];
        //
        if(debug_){
            m_tracker.saveRegisters(globalRegisters_, frame.base, frame.numReg);
        }
        if(instr.opcode > BASE_START && instr.opcode < BASE_END){
            processBaseInst(frame, instr);
        }else if(instr.opcode > CONTROL_START && instr.opcode < CONTROL_END){
            processControlInst(frame, instr);
        }else if(instr.opcode > OBJECT_START && instr.opcode < OBJECT_END){
            processObjectInst(frame, instr);
        }else{
            processOtherInst(frame, instr);
        }
        if(debug_){
            m_tracker.clearRegisters();
        }
    }
}

void VM::closeUpvaluesAbove(int stackIndex){
    std::vector<int> toRemove;
    for (auto& pair : openUpvalues_) {
        if (pair.first >= stackIndex) {
            pair.second->close();
            toRemove.push_back(pair.first);
        }
    }
    for (int index : toRemove) {
        openUpvalues_.erase(index);
    }
}
std::shared_ptr<Upvalue> VM::findOrCreateUpvalue(int stackIndex) {
    printf("findOrCreateUpvalue >> stackIndex = %d\n", stackIndex);
    if (openUpvalues_.find(stackIndex) != openUpvalues_.end()) {
        return openUpvalues_[stackIndex];
    }
    auto upvalue = std::make_shared<Upvalue>(&globalRegisters_[stackIndex]);
    openUpvalues_[stackIndex] = upvalue;
    return upvalue;
}
//-----------------------------------

void VM::processBaseInst(CallFrame& frame, const Instruction& instr){
    auto closure = frame.closure;

    switch (instr.opcode) {
    case LOADK: {
        auto& pool = const_pool_->at(frame.getModuleIndex());
        if (instr.b < (int)pool.size()) {
            getRegister(instr.a) = pool.getAt(instr.b);
        } else {
            std::cerr << "Constant index out of bounds: " << instr.b << std::endl;
        }
        frame.pc++;
        trackDiff(instr, {}, instr.a);
        break;
    }

    case LOADBOOL: {
        getRegister(instr.a) = static_cast<bool>(instr.b);
        frame.pc++;
        trackDiff(instr, {}, instr.a);
        break;
    }

    case MOVE: {
        getRegister(instr.a) = getRegister(instr.b);
        frame.pc++;
        trackDiff(instr, instr.b, instr.a);
        break;
    }

    case ADD: {
        auto& v1 = getRegister(instr.b);
        auto& v2 = getRegister(instr.c);
        auto td = m_factory.getTypeDelegate(v1.type);
        if(td != nullptr){
            getRegister(instr.a) = td->add(v1, v2);
        }
        frame.pc++;
        trackDiff(instr, {instr.b, instr.c}, instr.a);
        break;
    }

    case CONCAT: {
        auto& v1 = getRegister(instr.b);
        auto& v2 = getRegister(instr.c);
        auto td = m_factory.getTypeDelegate(v1.type);
        if(td != nullptr){
            getRegister(instr.a) = td->concat(v1, v2);
        }
        frame.pc++;
        trackDiff(instr, {instr.b, instr.c}, instr.a);
        break;
    }

    case SUB: {
        auto& v1 = getRegister(instr.b);
        auto& v2 = getRegister(instr.c);
        auto td = m_factory.getTypeDelegate(v1.type);
        if(td != nullptr){
            getRegister(instr.a) = td->sub(v1, v2);
        }
        frame.pc++;
        trackDiff(instr, {instr.b, instr.c}, instr.a);
        break;
    }

    case MUL: {
        auto& v1 = getRegister(instr.b);
        auto& v2 = getRegister(instr.c);
        auto td = m_factory.getTypeDelegate(v1.type);
        if(td != nullptr){
            getRegister(instr.a) = td->mul(v1, v2);
        }
        frame.pc++;
        trackDiff(instr, {instr.b, instr.c}, instr.a);
        break;
    }

    case DIV: {
        auto& v1 = getRegister(instr.b);
        auto& v2 = getRegister(instr.c);
        auto td = m_factory.getTypeDelegate(v1.type);
        if(td != nullptr){
            getRegister(instr.a) = td->div(v1, v2);
        }
        frame.pc++;
        trackDiff(instr, {instr.b, instr.c}, instr.a);
        break;
    }
//--------
    case EQ: {
        auto& v1 = getRegister(instr.b);
        auto& v2 = getRegister(instr.c);
        //
        getRegister(instr.a) = Value(v1.equals(v2));
        frame.pc++;
        trackDiff(instr, {instr.b, instr.c}, instr.a);
        break;
    }

    case LT: {
        auto& v1 = getRegister(instr.b);
        auto& v2 = getRegister(instr.c);
        auto td = m_factory.getTypeDelegate(v1.type);
        getRegister(instr.a) = td->lessThan(v1, v2);
        frame.pc++;
        trackDiff(instr, {instr.b, instr.c}, instr.a);
        break;
    }

    case LE: {
        auto& v1 = getRegister(instr.b);
        auto& v2 = getRegister(instr.c);
        auto td = m_factory.getTypeDelegate(v1.type);
        getRegister(instr.a) = td->lessEquals(v1, v2);
        frame.pc++;
        trackDiff(instr, {instr.b, instr.c}, instr.a);
        break;
    }

    }
}
void VM::processControlInst(CallFrame& frame, const Instruction& instr){
    auto closure = frame.closure;

    switch (instr.opcode) {
    case JMP:{
        //<t_pc>,-,-
        frame.pc = instr.a;
        trackDiff(instr);
    }break;

    case JMPIF:{
        //<t_pc>,<case>,-
        auto vb = getRegister(instr.b);
        if(vb.getBool()){
            frame.pc = instr.a;
        }else{
            frame.pc ++;
        }
        trackDiff(instr);
    }break;

    case JMPNOT:{
        //<t_pc>,<case>,-
        auto vb = getRegister(instr.b);
        if(!vb.getBool()){
            frame.pc = instr.a;
        }else{
            frame.pc ++;
        }
        trackDiff(instr);
    }break;

    case NEWLABEL:{
        closure->labelPcs.push_back(frame.pc);
        frame.pc++;
        trackDiff(instr);
    }break;

    case JMPLABEL:{
        //<lable-index>,-,-
        auto& v1 = getRegister(instr.a);
        auto idx = v1.getAsInt();
        if(idx < 0){
            idx = closure->labelPcs.size() + idx;
        }
        frame.pc = closure->labelPcs.at(idx) + 1;
        trackDiff(instr);
    }break;

    case JMPLABEL_IF:{
        //<lable-index>,<case>, -
        int idx;
        {
            auto& v1 = getRegister(instr.a);
            idx = v1.getAsInt();
            if(idx < 0){
                idx = closure->labelPcs.size() + idx;
            }
        }
        auto& v2 = getRegister(instr.b);
        if (v2.getBool()) {
            frame.pc = closure->labelPcs.at(idx) + 1;
        }else{
            frame.pc++;
        }
        trackDiff(instr);
    }break;

    case JMPLABEL_NOT:{
        //<lable-index>,<case>,-
        int idx;
        {
            auto& v1 = getRegister(instr.a);
            idx = v1.getAsInt();
            if(idx < 0){
                idx = closure->labelPcs.size() + idx;
            }
        }
        auto& v2 = getRegister(instr.b);
        if (!v2.getBool()) {
            frame.pc = closure->labelPcs.at(idx) + 1;
        }else{
            frame.pc++;
        }
        trackDiff(instr);
    }break;

    case RETURN: {
        // 返回到调用者
        closeUpvaluesAbove(frame.base);
        callStack_.pop();
        if (!callStack_.empty()) {
            CallFrame& caller = callStack_.top();
            caller.pc++; // 继续执行下一条指令
        }
        trackDiff(instr);
        break;
    }

    case CALL: {
        //closure, numParam, -
        auto& v1 = getRegister(instr.a);
        auto func = v1.getPtr<Closure>();
        if (func) {
            // 创建新的调用帧
            int newBase = frame.base + frame.getNumRegs();
            printf(" >> newBase = %d\n", newBase);
            //callStack.push(CallFrame(func, 0, newBase, func->proto->numRegisters));
            callStack_.emplace(func, 0, newBase, func->proto->numRegisters);
            // 复制参数.
            // for (int i = 0; i < func->proto->numParams && i < instr.b; i++) {
            //     globalRegisters_[newBase + i] = getRegister(instr.a + 1 + i);
            // }
        } else {
            std::cerr << "Type error: expected function for call\n";
            frame.pc++;
        }
        trackDiff(instr, instr.b, -1);
        break;
    }

    case CALL_C: {
        //<cfunc>,<target_val>,-
        auto& v1 = getRegister(instr.a);
        auto func = v1.getPtr<CFunction>();
        if(func){
            auto val = func->func(this, &frame);
            if(!val.isVoid()){
                getRegister(instr.b) = val;
            }
        }
        frame.pc++;
        trackDiff(instr, instr.a, -1);
        break;
    }

    case FORI_INIT:{
        //<init>, <end>, -
        auto& v1 = getRegister(instr.a);
        auto& v2 = getRegister(instr.b);

        auto ls = std::make_shared<LoopState>();
        ls->start = &v1.base.s32;
        ls->end = &v2.base.s32;
        ls->loopPc = frame.pc + 1;
        frame.loopStates.push(ls);
        frame.pc ++;
        trackDiff(instr, {instr.a, instr.b}, -1);
    }break;

    case FORP_INIT:{
        //<tab>, <reg_k>, <reg_v>
        auto& v1 = getRegister(instr.a);
        auto tab = v1.getPtr<Table>();
        auto it = tab->fields.begin();
        if(it != tab->fields.end()){
            getRegister(instr.b) = Value::makeString(it->first);
            getRegister(instr.c) = it->second;
        }else{
            getRegister(instr.b) = Value::makeNull();
            getRegister(instr.c) = Value::makeNull();
        }
        auto ls = std::make_shared<LoopState>();
        ls->iter_str = it;
        ls->tab = tab;
        ls->loopPc = frame.pc + 1;
        frame.loopStates.push(ls);
        frame.pc ++;
        trackDiff(instr, instr.a, -1);
    }break;

    case FORP_NEXT:{
        auto& v1 = getRegister(instr.a);
        auto tab = v1.getPtr<Table>();
        auto& ls = frame.loopStates.top();
        ls->iter_str++;
        if(ls->iter_str != tab->fields.end()){
            getRegister(instr.b) = Value::makeString(ls->iter_str->first);
            getRegister(instr.c) = ls->iter_str->second;
        }else{
            getRegister(instr.b) = Value::makeNull();
            getRegister(instr.c) = Value::makeNull();
        }
        frame.pc ++;
        trackDiff(instr, instr.a, -1);
    }break;

    case LOOP_BREAK:{
        int startCnt = 1;
        int endCnt = 0;
        auto& ls = frame.loopStates.top();
        for(int i = ls->loopPc; ; ++i){
            int op = frame.getInst(i).opcode;
            if(op == LOOP_END){
                endCnt ++;
                if(endCnt == startCnt){
                    frame.pc = i + 1;
                    break;
                }
            }else if(op == FORI_INIT || op == FORP_INIT){
                startCnt ++;
            }
        }
        if(startCnt != endCnt){
            MED_ASSERT_X(false, "LOOP_BREAK: ill-stat");
        }
        frame.loopStates.pop();
        trackDiff(instr);
    }break;

    case LOOP_CONTINUE:{
        auto& ls = frame.loopStates.top();
        frame.pc = ls->loopPc;
        trackDiff(instr);
    }break;
    }
}
void VM::processObjectInst(CallFrame& frame, const Instruction& instr){
    auto closure = frame.closure;

    switch (instr.opcode) {
    case NEWTABLE: {
        getRegister(instr.a) = Value::makeTable(new Table());
        frame.pc++;
        trackDiff(instr, {}, instr.a);
        break;
    }

    case GETTABLE: {
        //dst,tab,key
        auto& v1 = getRegister(instr.b); //table
        auto& v2 = getRegister(instr.c); //string or int-like

        auto tab = v1.getPtr<Table>();
        if(v2.isString()){
            auto str = v2.getPtr<StringRef>();
            auto dstV = tab->get(str->toString());
            if(dstV != nullptr){
                getRegister(instr.a) = *dstV;
            }else{
                getRegister(instr.a) = Value::makeNull();
            }
        }else if(v2.isIntLike()){
            auto dstV = tab->get(v2.getAsLong());
            if(dstV != nullptr){
                getRegister(instr.a) = *dstV;
            }else{
                getRegister(instr.a) = Value::makeNull();
            }
        }
        frame.pc++;
        trackDiff(instr, {instr.b, instr.c}, instr.a);
        break;
    }

    case SETTABLE: {
        //tab,k,v
        auto& tabV = getRegister(instr.a);
        auto& v1 = getRegister(instr.b);
        auto& v2 = getRegister(instr.c);

        auto tab = tabV.getPtr<Table>();
        if(v1.isIntLike()){
            auto key = v1.getAsLong();
            tab->set(key, v2);
        }else if(v1.isString()){
            auto key = v1.getPtr<StringRef>();
            tab->set(key->toString(), v2);
        }
        frame.pc++;
        trackDiff(instr, {instr.b, instr.c}, instr.a);
        break;
    }

    case NEWARRAY:{
        //dst, <type>, tab-as-array-shape,
        auto& reg_t = getRegister(instr.b);
        if(!reg_t.isIntLike()){
            //TODO wrong
            break;
        }
        auto& reg_info = getRegister(instr.c);
        if(reg_info.isTable()){
            //shape info is from table.
            auto tab = reg_info.getPtr<Table>();
            if(tab->getArraySize() > 0){
                List<int> shape;
                if(!tab->getArrayShapeDynamic(shape)){
                    //static
                    auto newArr = Array::New((Type)reg_t.getAsLong(), shape);
                    getRegister(instr.a) = Value::makeArray(newArr);
                }else{
                    //TODO shape is not fixed. must not reach here. should throw.
                }
            }
        }
        frame.pc++;
        trackDiff(instr, {instr.b, instr.c}, instr.a);
    }break;

    case SETARRAY:{
        //arr, index, val
        auto& reg_a = getRegister(instr.a);
        auto& reg_b = getRegister(instr.b);
        auto& reg_c = getRegister(instr.c);
        if(!reg_b.isIntLike()){
            //TODO wrong
            break;
        }
        if(reg_a.isArray()){
            auto ptr = reg_a.getPtr<Array>();
            ptr->setElement(reg_b.getAsLong(), &reg_c);
        }else{
            //throw ?
        }
        frame.pc++;
    }break;

    case GETARRAY:{
        //dst, arr, index
        auto& reg_b = getRegister(instr.b);
        auto& reg_c = getRegister(instr.c);
        if(!reg_c.isIntLike()){
            //TODO wrong
            break;
        }
        if(reg_b.isArray()){
            auto ptr = reg_b.getPtr<Array>();
            getRegister(instr.a) = ptr->getElement(reg_c.getAsLong());
        }else{
            //throw?
        }
        frame.pc++;
    }break;

    case CLOSURE: {
        // 创建闭包。 dst, nestedIdx, -
        if (instr.b < (int)closure->proto->nestedFunctions.size()) {
            auto nestedProto = closure->proto->nestedFunctions[instr.b];
            std::unique_ptr<Closure,ClosureDeleter> newClosure(new Closure(nestedProto));

            // 设置upvalues
            for (int i = 0; i < (int)nestedProto->upvDescs.size(); i++) {
                auto& desc = nestedProto->upvDescs[i];
                if (desc.first) {
                    // 父函数的局部变量
                    newClosure->upvalues[i] = findOrCreateUpvalue(frame.base + desc.second);
                } else {
                    // 父函数的upvalue
                    newClosure->upvalues[i] = closure->upvalues[desc.second];
                }
            }
            auto val_clo = Value::makeClosure(newClosure.release());
            getRegister(instr.a) = val_clo;
        } else {
            std::cerr << "Nested function index out of bounds: " << instr.b << std::endl;
        }
        frame.pc++;
        trackDiff(instr, {}, instr.a);
        break;
    }

    case GETUPVAL: {
        //dst, upindex, -
        if(instr.b < (int)closure->upvalues.size()){
            getRegister(instr.a) = *(closure->upvalues[instr.b]->get());
        }else{
            std::cerr << "GETUPVAL >> index out of bounds: " << instr.b << std::endl;
        }
        //
        frame.pc++;
        trackDiff(instr, {}, instr.a);
        break;
    }

    case SETUPVAL: {
        //dst-upval, upindex, -
        if(instr.b < (int)closure->upvalues.size()){
            closure->upvalues[instr.b]->set(getRegister(instr.a));
        }else{
            std::cerr << "SETUPVAL >> index out of bounds: " << instr.b << std::endl;
        }
        //
        frame.pc++;
        trackDiff(instr, instr.a, -1);
        break;
    }
    }
}
void VM::processOtherInst(CallFrame& frame, const Instruction& instr){
    auto closure = frame.closure;

    switch (instr.opcode) {
    case PRINT: {
        std::cout << "PRINT: ";
        getRegister(instr.a).print();
        frame.pc++;
        break;
    }

    case HALT: {
        running = false;
        break;
    }

    default: {
        std::cerr << "Unknown opcode: " << instr.opcode << std::endl;
        frame.pc++;
        break;
    }
    }
}


void VM::trackDiff(const Instruction& ins, CList<int> srcRegs, int dstReg){
    if(debug_){
        m_tracker.trackDiff(ins, VMTracker::Item(srcRegs, dstReg));
    }
}
void VM::trackDiff(const Instruction& ins, int srcReg, int dstReg){
    if(debug_){
        m_tracker.trackDiff(ins, VMTracker::Item(srcReg, dstReg));
    }
}
void VM::trackDiff(const Instruction& ins){
    if(debug_){
        m_tracker.trackDiff(ins, VMTracker::Item());
    }
}

}}
