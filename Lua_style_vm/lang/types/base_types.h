#pragma once

#include "../Type.h"

namespace h7l {

struct FloatType : public Type{
    FloatType(){
        priType = kPriType_FLOAT;
        id = TYPE_ID_FLOAT;
    }
};

struct DoubleType : public Type{
    DoubleType(){
        priType = kPriType_DOUBLE;
        id = TYPE_ID_DOUBLE;
    }
};

struct CharType : public Type{
    CharType(){
        priType = kPriType_CHAR;
        id = TYPE_ID_CHAR;
    }
};

struct ShortType : public Type{
    ShortType(){
        priType = kPriType_SHORT;
        id = TYPE_ID_SHORT;
    }
};

struct IntType : public Type{
    IntType(){
        priType = kPriType_INT;
        id = TYPE_ID_INT;
    }
};

struct LongType : public Type{
    LongType(){
        priType = kPriType_LONG;
        id = TYPE_ID_LONG;
    }
};

struct StringType : public Type{
    StringType(){
        id = TYPE_ID_STRING;
    }
};

}
