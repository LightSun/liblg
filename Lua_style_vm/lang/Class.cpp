#include "Type.h"
#include "Class.h"
#include "Object.h"
#include "utils/structs.h"
#include "GlobalContext.h"

using namespace h7l;

void Field::get(Object* obj, Value* out){
    char* _ptr = (char*)obj->getDataPtr();
    char* ptr = _ptr + offset;
    //TODO
    //out->deinit();
    if(shapes){
        //array.
    }
    if(type->isPrimetiveType()){

    }else{
        out->objPtr = (Object*)(ptr);
        out->weakRef = false;
        out->objPtr->ref();
    }
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
