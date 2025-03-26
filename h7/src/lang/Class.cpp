#include "src/lang/Class.h"
#include "src/Type.h"
#include "src/lang/GlobalContext.h"
#include "utils/structs.h"

using namespace h7l;

void Class::init(GlobalContext* gc, CList<Type*> types, CList<String> names){
    MED_ASSERT(types.size() == names.size());
    List<String> knames(types.size());
    for(size_t i = 0 ; i < types.size() ; ++i){
        knames[i] = types[i]->id;
    }
    auto layout = compute_struct_layout(knames, [gc](CString n, int cnt){
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
        f.type = types[i];
        f.name = names[i];
        f.offset = layout.members[i].offset;
    }
    this->fields = std::move(vec);
    this->structSize = layout.total_size;
}
