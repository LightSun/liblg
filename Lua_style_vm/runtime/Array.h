#pragma once

#include <memory>
#include <memory.h>
#include "runtime/Value.h"
#include "runtime/utils/h_atomic.h"
#include "runtime/utils/privates.h"

namespace h7l { namespace runtime {

using U32 = unsigned int;

struct ArrayDesc{
    List<int> shapes;
    List<int> strides;
    U32 baseSize {0};
    U32 eleCount {0}; //total

    ArrayDesc(U32 s): baseSize(s){}

    friend bool operator==(const ArrayDesc& o1, const ArrayDesc& o2){
        return o1.equals(o2);
    }

    int getElementCntPreDim(int dim)const{
        if(dim == -1){
            dim = shapes.size() - 1;
        }
        int cnt = 1;
        for(int i = 0 ; i < dim ; ++i){
            cnt *= shapes[i];
        }
        return cnt;
    }
    //incDim: include this dim or not.
    int getElementCntPostDim(int dim)const{
        int cnt = 1;
        for(int i = dim; i < (int)shapes.size() ; ++i){
            cnt *= shapes[i];
        }
        return cnt;
    }
    int getLastElementCnt()const{
        return shapes.back();
    }

    void removeOneEle(){
        MED_ASSERT(shapes.size()==1);
        --shapes[0];
        -- eleCount;
    }

    void setShape(CList<int> shapes){
        if(!shapes.empty()){
            this->shapes = shapes;
            strides.resize(shapes.size());
            const int size = shapes.size();
            U32 eleSize = baseSize;
            int eleC = 1;
            for(int i = size - 1 ; i >= 0 ; --i){
                strides[i] = eleSize;
                eleSize *= shapes[i];
                eleC *= shapes[i];
            }
            this->eleCount = eleC;
        }else{
            this->shapes.clear();
            strides.clear();
            eleCount = 0;
        }
    }
    bool toSubArray(int index, U32& offset, ArrayDesc& outArr){
        if(shapes.empty() || shapes[0] < index){
            return false;
        }
        int c = shapes[0];
        if(index < 0){
            index = c + index;
        }
        offset = index * strides[0];
        List<int> newShapes;
        for(size_t i = 1 ; i < shapes.size() ; ++i){
            newShapes.push_back(shapes[i]);
        }
        outArr.setShape(newShapes);
        return true;
    }
    void copyTo(ArrayDesc* dst){
        *dst = *this;
    }
    bool equals(const ArrayDesc& oth)const{
        if(this->baseSize != oth.baseSize){
            return false;
        }
        if(this->eleCount != oth.eleCount){
            return false;
        }
        if(this->shapes != oth.shapes){
            return false;
        }
        if(this->strides != oth.strides){
            return false;
        }
        return true;
    }

private:
    ArrayDesc(){}
};

struct ShareData{
    char* data {nullptr};

    ShareData(size_t alignSize){
        data = (char*)malloc(alignSize);
        memset(data, 0, alignSize);
    }
    ~ShareData(){
        if(data){
            free(data);
            data = nullptr;
        }
    }
};

struct Table;

//disable move and copy?
struct Array final: public BaseObjectType<Array>
{
private:
    Type eleType {kType_VOID};
    U32 offset {0};
    std::shared_ptr<ShareData> data;
    std::shared_ptr<ArrayDesc> desc;

public:
    struct DiffInfo{
        bool typeEq {false};
        int cnt {0}; // > 1 means n-eq. 0 means the same. 1 means can merge.
        int dimIndex {-1}; //dim index

        bool shouldMerge()const{
            return typeEq && cnt < 2;
        }
    };

    ~Array(){
        //printf("~Array\n");
    }

    static Array* New(Type eleType, CList<int> shapes);
    static Array* NewFromSimpleStr(CString str, Type eleType);

    void printTo(std::stringstream& ss)override;

    bool equals(IObjectType* _oth) const override;

    int hashCode()const override;

    int getBaseElementType()const{
        return eleType;
    }
    U32 getBaseElementCount()const{
        return desc->eleCount;
    }
    ArrayDesc* getArrayDesc()const{
        return desc.get();
    }
    int getDimCount()const{
        return desc->shapes.size();
    }
    bool isDataValid()const{
        return data != nullptr;
    }
    char* getDataPtr()const{
        return data->data + offset;
    }
    List<int>& getShapes(){
        return desc->shapes;
    }
    bool setElement(int index, Value* val,String* errorMsg = nullptr);

    //may be sub-arr
    Value getElement(int index){
        if(desc->strides.size() > 1){
            auto arr = subArray(index);
            if(arr != nullptr){
                return Value(kType_ARRAY, arr, false);
            }else{
                return Value::makeNull();
            }
        }else{
            if(pri_is_base_type(eleType)){
                char* srcPtr = getDataPtr() + index * pri_size(eleType);
                Value val;
                val.type = eleType;
                pri_cast(eleType, eleType, srcPtr, &val.base);
                return val;
            }else{
                auto ptr = ((IObjectType**)getDataPtr())[index];
                return Value(eleType, ptr, true);
            }
        }
    }
    Value getGlobalElement(int index){
        auto srcPtr = getGlobalElementPtr(index);
        //
        if(pri_isPointerLike(eleType)){
            return Value(eleType, (IObjectType*)srcPtr, true);
        }else{
            Value val;
            val.type = eleType;
            pri_cast(eleType, eleType, srcPtr, &val.base);
            return val;
        }
    }
    void setGlobalElement(int index, Value* val){
        char* ptr1 = getDataPtr();
        auto srcPtr = ptr1 + index * pri_size(eleType);
        if(pri_isPointerLike(eleType)){
            auto src = (IObjectType*)srcPtr;
            if(src){
                src->unref();
            }
            auto ptr = val->getPtr0();
            if(ptr){
                ptr->ref();
            }
            ((void**)ptr1)[index] = ptr;
        }else{
            pri_cast(val->type, eleType, &val->base, srcPtr);
        }
    }
    DiffInfo computeDiffForMerge(Array* arr){
        if(this->eleType != arr->eleType){
            return DiffInfo();
        }
        auto& shapes1 = getShapes();
        auto& shapes2 = arr->getShapes();
        if(shapes1.size() != shapes2.size()){
            return DiffInfo();
        }
        int diffCnt = 0;
        int diffI = -1;
        for(size_t i = 0 ; i < shapes1.size(); ++i){
            if(shapes1[i] != shapes2[i]){
                diffCnt ++;
                if(diffI < 0){
                    diffI = i;
                }
            }
        }
        DiffInfo info;
        info.typeEq = true;
        info.cnt = diffCnt;
        info.dimIndex = diffI;
        return info;
    }
    //2-3-1 + 2-3-2 -> 2-3-3

    //2-1-3 + 2-2-3 -> 2-3-3
    //   [1,2,3]     [1,2,3] [4,5,6]
    //   [4,5,6]     [7,8,9] [10,11,12]
    //result
    //   [1,2,3] [1,2,3] [4,5,6]
    //   [4,5,6] [7,8,9] [10,11,12]

    //1-2-3 + 2-2-3 -> 3-2-3
    //   [1,2,3] [4,5,6]      [1,2,3] [4,5,6]
    //                        [7,8,9] [10,11,12]
    // result
    // [1,2,3] [4,5,6]
    // [1,2,3] [4,5,6]
    // [7,8,9] [10,11,12]
    Value merge(Array* arr, int dim);

    //only support dim = 1
    //before call this, should call isElementComparable() first.
    Value removeSame(Array* arr);

    //only support dim = 1
    //before call this, should call isElementComparable() first.
    void sameIndexes(Array* arr, List<int>& v1, List<int>& v2);

    //only support dim = 1
    //before call this, should call isElementComparable() first.
    List<int> notContainsIndexes(Array* arr);

    //only support dim = 1
    //before call this, should call isElementComparable() first.
    List<int> sameIndexes(Array* arr);

    bool isElementComparable(Array* arr)const{
        if(pri_is_base_type(eleType)){
            if(pri_is_base_type(arr->eleType)){
                return true;
            }
            return false;
        }else{
            if(eleType == kType_NULL){
                return pri_isPointerLike(arr->eleType);
            }
            return eleType == arr->eleType;
        }
    }
    void copyTo(Array* arr){
        auto size = getBaseElementCount();
        for(U32 i = 0 ; i < size ; ++i){
            auto val = getGlobalElement(i);
            arr->setGlobalElement(i, &val);
        }
    }
    Array* copy(){
        std::unique_ptr<Array> arr = std::unique_ptr<Array>(
            Array::New(eleType, desc->shapes));
        copyTo(arr.get());
        return arr.release();
    }
private:
    int indexOfBase(int dstType,void* ptr2){
        for(size_t i = 0 ; i < desc->eleCount ; ++i){
            auto ptr1 = getGlobalElementPtr(i);
            if(pri_base_type_cmp(eleType, dstType, ptr1, ptr2) == 0){
                return i;
            }
        }
        return -1;
    }
    int indexOfPtr(void* ptr2){
        auto p2 = (IObjectType*)ptr2;
        for(size_t i = 0 ; i < desc->eleCount ; ++i){
            auto p1 = (IObjectType*)getGlobalElementPtr(i);
            if(p1 != nullptr){
                if(p2 && p1->equals(p2)){
                    return i;
                }
            }else if(p2 == nullptr){
                return i;
            }
        }
        return -1;
    }
    Array* subArray(int index){
        U32 curOffset = 0;
        const int unitSize = pri_size(eleType);
        auto newDesc = std::make_shared<ArrayDesc>(unitSize);
        if(desc->toSubArray(index, curOffset, *newDesc)){
            Array* array = new Array();
            array->eleType = this->eleType;
            array->offset = this->offset + curOffset;
            array->data = this->data;
            array->desc = newDesc;
            return array;
        }
        else{
            //index out of range
            return nullptr;
        }
    }
    char* getGlobalElementPtr(int index){
        MED_ASSERT(index < (int)desc->eleCount);
        char* ptr1 = getDataPtr();
        return ptr1 + index * pri_size(eleType);
    }
    void setGlobalElementPtr(int index, void* _srcP){
        MED_ASSERT(index < (int)desc->eleCount);
        if(pri_isPointerLike(eleType)){
            auto srcP = (IObjectType*)_srcP;
            char* ptr1 = getDataPtr();
            auto preVal = ((IObjectType**)ptr1)[index];
            if(preVal){
                preVal->unref();
            }
            if(srcP){
                srcP->ref();
            }
            ((IObjectType**)ptr1)[index] = srcP;
        }else{
            auto srcPtr = getGlobalElementPtr(index);
            pri_cast(eleType, eleType, _srcP, srcPtr);
        }
    }
    //only for 1D
    bool removeGlobalElementAt(int index){
        if(index >= (int)getArrayDesc()->eleCount){
            return false;
        }
        char* ptr1 = getDataPtr();
        if(pri_isPointerLike(eleType)){
            auto preVal = ((IObjectType**)ptr1)[index];
            if(preVal){
                preVal->unref();
                ((IObjectType**)ptr1)[index] = nullptr;
            }
        }
        auto baseSize = pri_size(eleType);
        auto moveSize = baseSize * (getArrayDesc()->eleCount - index - 1);
        auto srcOffset = baseSize * (index + 1);
        auto dstOffset = baseSize * index;
        memmove(ptr1 + dstOffset, ptr1 + srcOffset, moveSize);
        getArrayDesc()->removeOneEle();
        return true;
    }
    void delEleRef(){
        if(eleType != kType_VOID && !pri_is_base_type(eleType) && isDataValid()){
            auto ptr = (IObjectType**)getDataPtr();
            for(size_t i = 0 ; i < desc->eleCount ; ++i){
                if(ptr[i] != nullptr){
                    ptr[i]->unref();
                }
            }
            memset(ptr, 0, desc->eleCount * sizeof(void*));
        }
    }

    void printTo_default(std::stringstream& ss);
    void printTo_1(std::stringstream& ss);
    void printTo_2(std::stringstream& ss);
    void printTo_3(std::stringstream& ss);
    void printToImpl(std::stringstream& ss, int index);
    bool checkElementTypeMatch(int type, String* msg);
};


}}
