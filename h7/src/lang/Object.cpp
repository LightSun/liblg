#include <memory.h>
#include "Object.h"
#include "src/lang/Scope.h"
#include "src/lang/RuntimeContext.h"

using namespace h7l;

Object::Object(Scope* scope, Class* clsInfo, CList<int> shapes){
    MED_ASSERT_X(!clsInfo->isArrayType(), "clsInfo must not be array type.");
    std::unique_ptr<ArrayDesc> desc;
    if(!shapes.empty()){
        desc = std::make_unique<ArrayDesc>(clsInfo->structSize);
        desc->setShape(shapes);
    }
    init0(scope, clsInfo, nullptr, std::move(desc));
}
Object::Object(Scope* scope, Class* clsInfo, std::unique_ptr<ArrayDesc> desc){
    MED_ASSERT_X(!clsInfo->isArrayType(), "clsInfo must not be array type.");
    init0(scope, clsInfo, nullptr, std::move(desc));
}

Object::~Object(){
    //fprintf(stderr,"Object::~Object\n");
    unref();
}
void Object::unref(){
    if(h_atomic_add(&_ref, -1) == 1){
        mb.freeData();
        delete this;
    }
}
U32 Object::getDataSize(){
    auto clsInfo = getClass();
    if(clsInfo){
        if(arrayDesc){
            return arrayDesc->eleCount * clsInfo->structSize;
        }
        return clsInfo->structSize;
    }else{
        return mb.getPrimitiveSize();
    }
}
U32 Object::getBaseDataSize(){
    auto clsInfo = getClass();
    if(clsInfo){
        return clsInfo->structSize;
    }
    return mb.getPrimitiveSize();
}
void Object::reset(){
    mb.freeData();
    type = nullptr;
    super = nullptr;
    arrayDesc = nullptr;
    flags = 0;
}
Object* Object::subArray(int index){
    Object* obj = nullptr;
    if(isArray()){
        U32 offset = 0;
        auto newDesc = std::make_unique<ArrayDesc>(getBaseDataSize());
        if(arrayDesc->toSubArray(index, offset, *newDesc)){
            //
            // U32 actDataSize = newDesc->eleCount * clsInfo->structSize;
            // U32 alignSize;
            // if(clsInfo->isPrimiveType()){
            //     alignSize = actDataSize;
            // }else{
            //     alignSize = 8 - actDataSize % 8 + actDataSize;
            // }
            //
            obj = new Object();
            obj->mb.reset();
            ShareData* sd = (ShareData*)mb.data;
            sd->ref();
            size_t newOffset = mb.offset + offset;
            //
            obj->init0(scope, this->type, sd, std::move(newDesc));
            obj->mb.offset = newOffset;
            return obj;
        }else{
            fprintf(stderr, "index out of range.\n");
        }
    }
    return nullptr;
}
bool Object::castPrimitive(int priType){
    if(mb.castPrimitive(priType)){
        MED_ASSERT(scope);
        auto gc = scope->getGlobalContext();
        this->type = gc->getPrimitiveType(priType);
        return true;
    }
    return false;
}
bool Object::castPrimitiveTo(int priType, void* newPtr){
    if(mb.castPrimitiveTo(priType, newPtr)){
        MED_ASSERT(scope);
        auto gc = scope->getGlobalContext();
        this->type = gc->getPrimitiveType(priType);
        return true;
    }
    return false;
}
//-----------------------
void Object::init0(Scope* scope, Type* _type, ShareData* sd, std::unique_ptr<ArrayDesc> desc){
    MED_ASSERT_X(_type, "type must be valid.");
    Class* cls = nullptr;
    if(!_type->isPrimetiveType()){
        cls = (Class*)_type;
        MED_ASSERT_X(!cls->isArrayType(), "clsInfo must not be array type.");
    }
    this->scope = scope;
    this->type = _type;
    this->super = nullptr;
    this->_ref = 1;
    this->flags = 0;
    if(desc){
        arrayDesc = std::move(desc);
        const int unitSize = cls ? cls->structSize : primitive_get_size(_type->priType);
        U32 actDataSize;
        U32 alignSize;
        actDataSize = arrayDesc->eleCount * unitSize;
        if(_type->isPrimetiveType()){
            alignSize = actDataSize;
        }else{
            alignSize = sizeof(void*) - actDataSize % sizeof(void*) + actDataSize;
        }
        if(sd == nullptr){
            sd = ShareData::New(alignSize);
        }
        if(mb.data == nullptr){
            mb.initWithShareData(sd, actDataSize, alignSize);
        }
    }else{
        this->arrayDesc = nullptr;
        if(cls){
            mb.initWithStructSize(cls->structSize);
        }
    }
}
//
//------------------------------
static inline int _cmp_obj_impl(Object& ob1, Object& ob2, bool reverse){
    RuntimeContext rc(ob1.scope);
    ValueHolder val(&ob1, false);
    ValueHolder val2(&ob2, false);
    if(ob1.getType()->call(kFuncOp_compare, &rc, val.get(), val2.get())){
        MED_ASSERT_X(rc.result.objPtr, "Object::compare >> cmp with no result.");
        int ret = rc.result.objPtr->mb.getInt();
        if(!reverse){
            return ret;
        }else{
            return ret >= kCmpRet_LESS ? -ret : ret;
        }
    }else{
        return kCmpRet_ERROR_CANT_CMP;
    }
}
int Object::compare(Object& oth){
    if(type->isPrimetiveType()){
        if(oth.getType()->isPrimetiveType()){
            return mb.comparePrimitive(oth.mb);
        }
        return kCmpRet_ERROR_CANT_CMP;
    }else if(oth.getType()->isPrimetiveType()){
        return kCmpRet_ERROR_CANT_CMP;
    }else{
        auto c1 = asClass();
        auto c2 = oth.asClass();
        if(c1 == c2 || c1->hasSuperClass(c2)){
            return _cmp_obj_impl(*this, oth, false);
        }else if(c2->hasSuperClass(c1)){
            return _cmp_obj_impl(oth, *this, true);
        }else{
            return kCmpRet_ERROR_CANT_CMP;
        }
    }
}

