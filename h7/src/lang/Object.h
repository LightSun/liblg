#pragma once

#include "src/lang/MemoryBlock.h"
#include "src/Value.h"

namespace h7l {

struct Class;
struct Allocator;
class Scope;

struct ArrayDesc{
    List<int> shapes;
    List<int> strides;
    U32 baseSize {0};
    U32 eleCount {0};

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
};

struct Object: public IObject{

    MemoryBlock mb;
    Scope* scope {nullptr};
    Type* clsInfo {nullptr}; //baseType
    Object* super {nullptr};
    volatile int _ref {1};
    U32 flags {0};
    std::unique_ptr<ArrayDesc> arrayDesc;

    Object(Scope* scope, Class* clsInfo, CList<int> shapes = {});
    Object(Scope* scope, Class* clsInfo, std::unique_ptr<ArrayDesc> desc);
    Object(){}
    ~Object();

    void ref(){
        h_atomic_add(&_ref, 1);
    }
    //fetch_add
    void unref() override;
    bool hasFlag(U32 flag)const{ return (flags & flag) == flag;}
    void addFlag(U32 flag){ flags |= flag;}
    void* getDataPtr(){return mb.getDataPtr();}
    U32 dataSize();
    bool isArray(){return arrayDesc && arrayDesc->eleCount > 0;}
    //
    void setStringAsData(CString buf){
        mb.setStringAsData(buf);
    }
    Object* subArray(int index);

private:
    void init0(Scope* scope, Class* clsInfo, ShareData* sd,
               std::unique_ptr<ArrayDesc> desc);
};

}
