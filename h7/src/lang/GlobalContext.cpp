#include "GlobalContext.h"
#include "src/types/FloatType.h"
#include "src/types/IntType.h"

using namespace h7l;

GlobalContext::GlobalContext(){
    putRawType<DoubleType>();
    putRawType<FloatType>();
    putRawType<IntType>();
    putRawType<ShortType>();
    putRawType<CharType>();
    putRawType<LongType>();
}
