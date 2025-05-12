#include <unordered_map>
#include <cstring>
#include "src/Value.h"
#include "src/lang/Object.h"

using namespace h7l;

Type* Value::getType(){
    return objPtr ? (Type*)objPtr->clsInfo : nullptr;
}
void Value::deinit(){
    if(!temp && objPtr){
        objPtr->unref();
        objPtr = nullptr;
    }
}
