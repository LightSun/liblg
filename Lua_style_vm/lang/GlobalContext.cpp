#include "GlobalContext.h"
#include "types/base_types.h"

using namespace h7l;

GlobalContext::GlobalContext(){
    putRawType<DoubleType>();
    putRawType<FloatType>();
    putRawType<IntType>();
    putRawType<ShortType>();
    putRawType<CharType>();
    putRawType<LongType>();
    putRawType<StringType>();
}
int GlobalContext::compare(Object* o1, Object* o2){

    //TODO latter impl.
    return kCmpRet_ERROR_CANT_CMP;
}
