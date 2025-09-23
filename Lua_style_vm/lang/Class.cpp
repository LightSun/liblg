#include "Type.h"
#include "Class.h"
#include "Object.h"
#include "utils/structs.h"
#include "Scope.h"

using namespace h7l;

//object: free
//array: share
//
Object* Field::getCopy(Object* obj){
    char* ptr = (char*)obj->getDataPtr() + offset;
    Object* newObj;
    if(type->isArrayType()){
        if(type->isPrimetiveType()){
            newObj = new Object(obj->scope, type->priType, *shapes);
            void* dstPtr = newObj->getDataPtr();
            memcpy(dstPtr, ptr, newObj->getDataSize());
        }else{
            auto baseType = obj->scope->getGlobalContext()->getType(type->id);
            newObj = new Object(obj->scope, baseType, *shapes);
            Object** dstPtr = (Object**)newObj->getDataPtr();
            //
            auto srcPtr = (Object**)(ptr);
            int c = getTotalElementCount();
            for(int i = 0 ; i < c ; ++i){
                Object* src = (Object*)(srcPtr[i]);
                dstPtr[i] = src->copyDeep();
            }
        }
    }else if(type->isPrimetiveType()){
        newObj = Object::newPrimitive(obj->scope, type->priType);
        primitive_cast(type->priType, type->priType, ptr, newObj->getDataPtr());
    }else{
       auto val = (Object*)(ptr);
       newObj = val->copy();
    }
    return newObj;
}
void Field::set(Object* dstObj, Object* inObj){
    char* dstPtr = (char*)dstObj->getDataPtr() + offset;
    char* srcPtr = (char*)inObj->getDataPtr();
    MED_ASSERT(type == inObj->getType());
    MED_ASSERT(dstObj->getDataSize() == inObj->getDataSize());

    if(type->isArrayType()){
        MED_ASSERT(inObj->getType()->isArrayType());
        if(type->isPrimetiveType()){
            MED_ASSERT(inObj->getType()->priType == dstObj->getType()->priType);
            memcpy(dstPtr, inObj->getDataPtr(), inObj->getDataSize());
        }else{
            auto c = inObj->getTotalElementCount();
            Object** sd = (Object**)inObj->getDataPtr();
            Object** old_sd = (Object**)dstPtr;
            for(int i = 0 ; i < c; ++i){
                sd[i]->ref();
            }
            for(int i = 0 ; i < c; ++i){
                if(old_sd[i]){
                    old_sd[i]->unref();
                }
                old_sd[i] = sd[i];
            }
        }
    }else if(type->isPrimetiveType()){
        auto dataSize = primitive_get_size(type->priType);
        memcpy(dstPtr, srcPtr, dataSize);
    }else{
        Object* ptr = (Object*)dstPtr;
        if(ptr){
            ptr->unref();
        }
        ptr = inObj;
    }
}
void Field::get(Object* obj, Value* out){
    char* ptr = (char*)obj->getDataPtr() + offset;
    //
    out->deinit();
    Object* newObj;
    if(type->isArrayType()){
        MED_ASSERT(obj->isArray());
        auto sd = obj->mb.getShareData();
        auto baseType = obj->scope->getGlobalContext()->getType(type->id);
        newObj = new Object(obj->scope, baseType, *shapes);
        newObj->mb.refShareData(sd);
    }else if(type->isPrimetiveType()){
        newObj = Object::newPrimitive(obj->scope, type->priType);
        primitive_cast(type->priType, type->priType, ptr, newObj->getDataPtr());
    }else{
        newObj = (Object*)(ptr);
        newObj->ref();
    }
    out->objPtr = newObj;
    out->weakRef = false;
}

void Class::init(GlobalContext* gc, CList<MemberInfo> types, CList<String> names){
    MED_ASSERT(types.size() == names.size());
    List<MemberInfo0> infos(types.size());
    for(size_t i = 0 ; i < types.size() ; ++i){
        infos[i].baseType = types[i].type->id;
        infos[i].shapes = types[i].shapes;
    }
    auto layout = compute_struct_layout2(infos, [gc](CString n, int cnt){
            auto type = gc->getClass(n);
            MED_ASSERT_F(type, "can't find type/Class = '%s'", n.data());
            TypeInfo ti;
            ti.size = type->structSize * cnt;
            ti.alignment = sizeof(void*); //default 8
            return ti;
    });
    List<Field> vec(layout.members.size());
    for(int i = 0 ; i < (int)vec.size(); ++i){
        auto& f = vec[i];
        f.type = types[i].type;
        f.name = names[i];
        f.offset = layout.members[i].offset;
        f.setShapes(layout.members[i].shapes);
    }
    this->fields = std::move(vec);
    this->structSize = layout.total_size;
}
