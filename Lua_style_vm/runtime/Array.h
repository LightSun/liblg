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
    bool equals(ArrayDesc& oth){
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
    ~Array(){}

    static Array* New(Type eleType, CList<int> shapes){
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

    void printTo(std::stringstream& ss)override;

    bool equals(IObjectType* _oth) const override;

    ArrayDesc* getArrayDesc(){
        return desc.get();
    }

    bool isDataValid()const{
        return data != nullptr;
    }
    char* getDataPtr()const{
        return data->data + offset;
    }
    List<int>& shapes(){
        return desc->shapes;
    }
    //false if type not match.(base: non-base)
    bool setElement(int index, Value* val){
        int stride = desc->strides[0];
        char* ptr1 = (getDataPtr() + stride);
        if(pri_is_base_type(val->type)){
            if(!pri_is_base_type(eleType)){
                return false;
            }
            char* dstPtr = ptr1 + index * pri_size(eleType);
            pri_cast(val->type, eleType, &val->base, dstPtr);
        }else{
            if(pri_is_base_type(eleType)){
                return false;
            }
            auto ptr = val->getPtr0();
            if(ptr){
                ptr->ref();
            }
            ((void**)ptr1)[index] = ptr;
        }
        return true;
    }

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
                char* srcPtr = (getDataPtr() + desc->strides[0]) + index * pri_size(eleType);
                Value val;
                val.type = eleType;
                pri_cast(eleType, eleType, srcPtr, &val.base);
                return val;
            }else{
                char* dstPtr = (getDataPtr() + desc->strides[0]);
                auto ptr = ((IObjectType**)dstPtr)[index];
                return Value(eleType, ptr, true);
            }
        }
    }

private:
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
    void delEleRef(){
        if(eleType != kType_VOID && !pri_is_base_type(eleType) && isDataValid()){
            auto ptr = (IObjectType**)getDataPtr();
            for(int i = 0 ; i < desc->eleCount ; ++i){
                if(ptr[i] != nullptr){
                    ptr[i]->unref();
                }
            }
            memset(ptr, 0, desc->eleCount * sizeof(void*));
        }
    }
};


}}
