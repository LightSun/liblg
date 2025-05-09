
#include <unordered_map>
#include "cons.h"

namespace h7l {

int primitive_get_size(int priType){
    static std::unordered_map<int,int> s_pri_sizeMap = {
        {kPriType_BOOL, sizeof(char)},
        {kPriType_CHAR, sizeof(char)},
        {kPriType_UCHAR, sizeof(unsigned char)},
        {kPriType_SHORT, sizeof(short)},
        {kPriType_USHORT, sizeof(unsigned short)},
        {kPriType_INT, sizeof(int)},
        {kPriType_UINT, sizeof(unsigned int)},
        {kPriType_LONG, sizeof(long long)},
        {kPriType_ULONG, sizeof(unsigned long long)},
        {kPriType_FLOAT, sizeof(float)},
        {kPriType_DOUBLE, sizeof(double)},
    };
    auto it = s_pri_sizeMap.find(priType);
    return it != s_pri_sizeMap.end() ? it->second : -1;
}
}
