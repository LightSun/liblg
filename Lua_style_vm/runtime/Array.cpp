#include <memory.h>
#include <iostream>
#include "runtime/Array.h"
#include "runtime/utils/ArrayParser.h"
//#include "runtime/common_base.h"

namespace h7l { namespace runtime {



Array* Array::New(Type eleType, CList<int> shapes){
    const int unitSize = pri_size(eleType);
    auto desc = std::make_shared<ArrayDesc>(unitSize);
    desc->setShape(shapes);
    auto actDataSize = desc->eleCount * unitSize;
    const int ptrSize = sizeof (void*);
    int left = actDataSize % ptrSize;
    size_t alignSize;
    if(left == 0){
        alignSize = actDataSize;
    }else{
        alignSize = actDataSize + ptrSize - left;
    }
    Array* array = new Array();
    auto sd = std::shared_ptr<ShareData>(new ShareData(alignSize), [array](ShareData*){
        array->delEleRef();
    });
    array->eleType = eleType;
    array->data = sd;
    array->desc = desc;
    return array;
}

Array* Array::NewFromSimpleStr(CString str0, Type eleType){
    auto str = SimpleArrayParser::removeBlank(str0);
    int dimC = SimpleArrayParser::getArrayDimCnt(str);
    List<int> shapes;
    switch (eleType) {
    case kType_S32:{
        List<int> eles;
        if(SimpleArrayParser::parseInt(str, dimC, shapes, eles)){
            auto arr = New(eleType, shapes);
            for(int i = 0 ;i < (int)eles.size(); ++i){
                arr->setGlobalElementPtr(i, &eles[i]);
            }
            return arr;
        }else{
            return nullptr;
        }
    }break;

    case kType_FLOAT:{
        List<float> eles;
        if(SimpleArrayParser::parseFloat(str, dimC, shapes, eles)){
            auto arr = New(eleType, shapes);
            for(int i = 0 ;i < (int)eles.size(); ++i){
                arr->setGlobalElementPtr(i, &eles[i]);
            }
            return arr;
        }else{
            return nullptr;
        }
    }break;

    case kType_DOUBLE:{
        List<double> eles;
        if(SimpleArrayParser::parseDouble(str, dimC, shapes, eles)){
            auto arr = New(eleType, shapes);
            for(int i = 0 ;i < (int)eles.size(); ++i){
                arr->setGlobalElementPtr(i, &eles[i]);
            }
            return arr;
        }else{
            return nullptr;
        }
    }break;

    default:
        //
        fprintf(stderr, "current not support type = %d\n", eleType);
        return nullptr;
    }
}

bool Array::setElement(int index, Value* val,String* errorMsg){
    if(desc->strides.size() > 1){
        if(!val->isArray()){
            if(errorMsg){
                *errorMsg = "Array: setElement need a sub-array. but provide is not.";
            }
            return false;
        }
        if(!val->isNullptr() && !checkElementTypeMatch(
                val->getPtr<Array>()->getBaseElementType(), errorMsg)){
            return false;
        }
        if(index >= getShapes()[0]){
            if(errorMsg){
                char buf[256];
                snprintf(buf, 256, "Array: index(%d) out of range(%d).",
                         index, getShapes()[0]);
                *errorMsg = buf;
            }
            return false;
        }
        auto inArr = val->getPtr<Array>();
        //sub arr copy.
        auto subArr = subArray(index);
        if(subArr){
            if(subArr->getBaseElementCount() == inArr->getBaseElementCount()){
                inArr->copyTo(subArr);
                return true;
            }else{
                if(errorMsg){
                    *errorMsg = "Array: base-element-count not-equals.";
                }
            }
        }else{
            if(errorMsg){
                *errorMsg = "Array: current array is 1d.";
            }
        }
        return false;
    }else{
        if(!checkElementTypeMatch(val->type, errorMsg)){
            return false;
        }
        if(index < (int)getBaseElementCount()){
            setGlobalElement(index, val);
        }else{
            if(errorMsg){
                char buf[256];
                snprintf(buf, 256, "Array: index(%d) out of range(%d).",
                         index, getBaseElementCount());
                *errorMsg = buf;
            }
        }
    }
    return true;
}

//dim = 2
//2-3-1 + 2-3-2 -> 2-3-3
//  [1] [2] [3]       [1,2] [3,4]  [5,6]
//  [4] [5] [6]       [7,8] [9,10] [11,12]
//result
//  [1,1,2] [2,3,4] [3,5,6]
//  [4,7,8] [5,9,10] [6,11,12]

//dim = 1
//2-1-3 + 2-2-3 -> 2-3-3
//   [1,2,3]     [1,2,3] [4,5,6]
//   [4,5,6]     [7,8,9] [10,11,12]
//result
//   [1,2,3] [1,2,3] [4,5,6]
//   [4,5,6] [7,8,9] [10,11,12]

//dim = 0
//1-2-3 + 2-2-3 -> 3-2-3
//   [1,2,3] [4,5,6]      [1,2,3] [4,5,6]
//                        [7,8,9] [10,11,12]
// result
// [1,2,3] [4,5,6]
// [1,2,3] [4,5,6]
// [7,8,9] [10,11,12]
Value Array::merge(Array* arr, int dim){ //dim -> 2-3-1 -> 0 means 1, 2 means 2
    if(dim < 0){
        dim = 0;
    }
    //
    auto tshapes = getShapes();
    tshapes[dim] += arr->getShapes()[dim];
    //
    auto karr = std::unique_ptr<Array>(New(eleType, tshapes));
    if(dim == 0){
        //max dim
        auto curSize = getArrayDesc()->eleCount;
        for(size_t i = 0; i < karr->desc->eleCount; ++i){
            if(i < curSize){
                auto curPtr = getGlobalElementPtr(i);
                karr->setGlobalElementPtr(i, curPtr);
            }else{
                auto curPtr = arr->getGlobalElementPtr(i - curSize);
                karr->setGlobalElementPtr(i, curPtr);
            }
        }
    }else if(dim == (int)tshapes.size() - 1){
        //lowest dim
        auto tStride = karr->getArrayDesc()->getLastElementCnt();
        auto curStride = getArrayDesc()->getLastElementCnt();
        auto othStride = arr->getArrayDesc()->getLastElementCnt();
        int tCnt = karr->getArrayDesc()->getElementCntPreDim(-1);
        //
        for(int lp = 0 ; lp < tCnt ; ++lp){
            int baseDstIdx = lp * tStride;
            for(int i = 0 ; i < curStride; ++i){
                int srcIdx = lp * curStride + i;
                int dstIdx = baseDstIdx + i;
                auto curPtr = getGlobalElementPtr(srcIdx);
                karr->setGlobalElementPtr(dstIdx, curPtr);
            }
            for(int i = 0 ; i < othStride; ++i){
                int srcIdx = lp * othStride + i;
                int dstIdx = baseDstIdx + curStride + i;
                auto curPtr = arr->getGlobalElementPtr(srcIdx);
                karr->setGlobalElementPtr(dstIdx, curPtr);
            }
        }
    }else{
        int tCnt = karr->getArrayDesc()->getElementCntPreDim(dim);
        auto curEvCnt = getArrayDesc()->getElementCntPostDim(dim);
        auto othEvCnt = arr->getArrayDesc()->getElementCntPostDim(dim);
        int tcvCnt = karr->getArrayDesc()->eleCount / tCnt;
        for(int lp = 0 ; lp < tCnt ; ++lp){
            int baseDstIdx = lp * tcvCnt;
            for(int i = 0 ; i < curEvCnt; ++i){
                int srcIdx = lp * curEvCnt + i;
                int dstIdx = baseDstIdx + i;
                auto curPtr = getGlobalElementPtr(srcIdx);
                karr->setGlobalElementPtr(dstIdx, curPtr);
            }
            baseDstIdx += curEvCnt;
            for(int i = 0 ; i < othEvCnt; ++i){
                int srcIdx = lp * othEvCnt + i;
                int dstIdx = baseDstIdx + i;
                auto curPtr = arr->getGlobalElementPtr(srcIdx);
                karr->setGlobalElementPtr(dstIdx, curPtr);
            }
        }
    }
    return Value::makeArray(karr.release());
}

Value Array::removeSame(Array* arr){
    auto vec1 = notContainsIndexes(arr);
    //
    auto karr = std::unique_ptr<Array>(New(eleType, {(int)vec1.size()}));
    for(size_t i = 0 ; i < vec1.size(); ++i){
        int& id = vec1[i];
        auto curPtr = arr->getGlobalElementPtr(id);
        karr->setGlobalElementPtr(i, curPtr);
    }
    return Value::makeArray(karr.release());
}

List<int> Array::notContainsIndexes(Array* arr){
    List<int> vec1;
    if(pri_is_base_type(eleType)){
        for(int i = 0 ; i < (int)desc->eleCount ; ++i){
            auto p1 = getGlobalElementPtr(i);
            auto idx = arr->indexOfBase(eleType, p1);
            if(idx < 0){
                vec1.push_back(i);
            }
        }
    }else{
        for(int i = 0 ; i < (int)desc->eleCount ; ++i){
            auto p1 = getGlobalElementPtr(i);
            auto idx = arr->indexOfPtr(p1);
            if(idx < 0){
                vec1.push_back(i);
            }
        }
    }
    return vec1;
}
void Array::sameIndexes(Array* arr, List<int>& v1, List<int>& v2){
    if(pri_is_base_type(eleType)){
        for(int i = 0 ; i < (int)desc->eleCount ; ++i){
            auto p1 = getGlobalElementPtr(i);
            auto idx = arr->indexOfBase(eleType, p1);
            if(idx >= 0){
                v1.push_back(i);
                v2.push_back(idx);
            }
        }
    }else{
        for(int i = 0 ; i < (int)desc->eleCount ; ++i){
            auto p1 = getGlobalElementPtr(i);
            auto idx = arr->indexOfPtr(p1);
            if(idx >= 0){
                v1.push_back(i);
                v2.push_back(idx);
            }
        }
    }
}
List<int> Array::sameIndexes(Array* arr){
    List<int> vec1; //cur
    List<int> vec2; //oth
    sameIndexes(arr, vec1, vec2);
    if(!vec1.empty()){
        vec1.insert(vec1.end(), -1);
        vec1.insert(vec1.end(), vec2.begin(), vec2.end());
        return vec1;
    }
    return {};
}

void Array::printTo(std::stringstream& ss){
    if(isDataValid()){
        switch (getArrayDesc()->shapes.size()) {
        case 1:{
            printTo_1(ss);
        }break;
        case 2:{
            printTo_2(ss);
        }break;
        case 3:{
            printTo_3(ss);
        }break;
        default:
            printTo_default(ss);
        }
    }else{
        ss << "[]";
    }
}

bool Array::equals(IObjectType* _oth) const{
    if(this == _oth){
        return true;
    }
    auto oth = dynamic_cast<Array*>(_oth);
    if(oth == nullptr) return false;
    if(!isDataValid() || !oth->isDataValid()){
        return false;
    }
    if(this->eleType != oth->eleType){
        return false;
    }
    if(!this->desc->equals(*oth->desc)){
        return false;
    }
    if(pri_is_base_type(eleType)){
        return pri_base_type_array_eqs(eleType, getDataPtr(),
                                       oth->getDataPtr(), desc->eleCount);
    }else{
        auto ptr = (IObjectType**)getDataPtr();
        auto ptr2 = (IObjectType**)oth->getDataPtr();
        for(int i = 0 ; i < (int)desc->eleCount ; ++i){
            if(ptr[i] == nullptr){
                if(ptr2[i] != nullptr){
                    return false;
                }else{
                    continue;
                }
            }
            if(!ptr[i]->equals(ptr2[i])){
                return false;
            }
        }
        return true;
    }
}
int Array::hashCode()const{
    int var = 0;
    if(pri_is_base_type(eleType)){
        auto eleSize = pri_size(eleType);
        auto ptr = getDataPtr();
        for(int i = 0 ; i < (int)desc->eleCount ; ++i){
            auto dstPtr = ptr + i * eleSize;
            var = 31 * var + base_hash(eleType, dstPtr);
        }
    }else{
        auto ptr = (IObjectType**)getDataPtr();
        for(int i = 0 ; i < (int)desc->eleCount ; ++i){
            if(ptr[i]){
                var = 31 * var + ptr[i]->hashCode();
            }else{
                var = 31 * var;
            }
        }
    }
    return var;
}
void Array::printTo_default(std::stringstream& ss){
    ss << "[";
    if(pri_is_base_type(eleType)){
        pri_base_type_array_print(ss, eleType, getDataPtr(), desc->eleCount);
    }else{
        ss << CMD_LINE;
        auto ptr = (IObjectType**)getDataPtr();
        for(size_t i = 0 ; i < desc->eleCount ; ++i){
            if(ptr[i]){
                ptr[i]->printTo(ss);
            }else{
                ss << "null";
            }
            if(i != desc->eleCount - 1){
                ss << ", ";
            }
            ss << CMD_LINE;
        }
    }
    ss << "]";
}
void Array::printTo_1(std::stringstream& ss){
    ss << "[";
    auto& shapes = getShapes();
    for (int i = 0; i < shapes[0]; ++i) {
        printToImpl(ss, i);
        if (i < shapes[0] - 1) ss << ",";
    }
    ss << "]";
}
void Array::printTo_2(std::stringstream& ss){
    ss << "[" << CMD_LINE;
    auto& shapes = getShapes();
    auto& strides = getArrayDesc()->strides;
    const auto baseSize = pri_size(eleType);
    for (int i = 0; i < shapes[0]; ++i) {
        ss << " [";
        //
        int offset1 = strides[0] / baseSize * i;
        for (int j = 0; j < shapes[1]; ++j) {
            printToImpl(ss, offset1 + j);
            if (j < shapes[1] - 1){
                ss << ",";
            }
        }
        ss << "]";
        if (i < shapes[0] - 1) ss << ",";
        ss << CMD_LINE;
    }
    ss << "]";
}
void Array::printTo_3(std::stringstream& ss){
    ss << "[" << CMD_LINE;
    auto& shapes = getShapes();
    auto& strides = getArrayDesc()->strides;
    const auto baseSize = pri_size(eleType);
    //
    for (int i = 0; i < shapes[0]; ++i) {
        ss << " [" << CMD_LINE;
        //
        int offset1 = strides[0] / baseSize * i;
        for (int j = 0; j < shapes[1]; ++j) {
            ss << "  [";
            int offset2 = offset1 + strides[1] / baseSize * j;
            for (int k = 0; k < shapes[2]; ++k) {
                printToImpl(ss, offset2 + k);
                if (k < shapes[2] - 1){
                    ss << ",";
                }
            }
            ss << "]";
            if (j < shapes[1] - 1){
                ss << ",";
            }
            ss << CMD_LINE;
        }
        ss << " ]";
        if (i < shapes[0] - 1) ss << ",";
        ss << CMD_LINE;
    }
    ss << "]";
}
void Array::printToImpl(std::stringstream& ss, int index){
    auto ptr = getGlobalElementPtr(index);
    val_printRaw(eleType, ptr, ss);
}
bool Array::checkElementTypeMatch(int othType, String* errorMsg){
    bool base1 = pri_is_base_type(eleType);
    bool base2 = pri_is_base_type(othType);
    if(base1 && !base2){
        if(errorMsg){
            *errorMsg = "Array: checkTypeMatch() type mismatch(base1 && !base2).";
        }
        return false;
    }
    if(!base1 && base2){
        if(errorMsg){
            *errorMsg = "Array: checkTypeMatch() type mismatch(!base1 && base2).";
        }
        return false;
    }
    //extend ?
//    if(!base1 && !base2 && othType != kType_NULL && eleType != othType){
//        if(errorMsg){
//            *errorMsg = "Array: checkTypeMatch() type mismatch(!base1 && base2).";
//        }
//        return false;
//    }
    return true;
}

}}
