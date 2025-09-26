#include "runtime/TypeDelegateFactory.h"
#include "runtime/impl/BaseTypeDelegates.h"
#include "runtime/impl/BoolTypeDelegate.h"
#include "runtime/impl/StringTypeDelegate.h"
#include "runtime/impl/TableTypeDelegate.h"
#include "runtime/impl/CFunctionTypeDelegate.h"
#include "runtime/impl/ClosureTypeDelegate.h"

namespace h7l { namespace runtime {

TypeDelegateFactory::TypeDelegateFactory(){

    m_map[kType_S8] = std::make_shared<CharTypeDelegate>();
    m_map[kType_U8] = std::make_shared<UCharTypeDelegate>();
    m_map[kType_S16] = std::make_shared<ShortTypeDelegate>();
    m_map[kType_U16] = std::make_shared<UShortTypeDelegate>();
    m_map[kType_S32] = std::make_shared<IntTypeDelegate>();
    m_map[kType_U32] = std::make_shared<UIntTypeDelegate>();
    m_map[kType_S64] = std::make_shared<LongTypeDelegate>();
    m_map[kType_U64] = std::make_shared<ULongTypeDelegate>();
    m_map[kType_FLOAT] = std::make_shared<FloatTypeDelegate>();
    m_map[kType_DOUBLE] = std::make_shared<DoubleTypeDelegate>();
    m_map[kType_BOOL] = std::make_shared<BoolTypeDelegate>();

    m_map[kType_STRING] = std::make_shared<StringTypeDelegate>();
    m_map[kType_TABLE] = std::make_shared<TableTypeDelegate>();
    m_map[kType_CLOSURE] = std::make_shared<ClosureTypeDelegate>();
    m_map[kType_CFUNCTION] = std::make_shared<CFunctionTypeDelegate>();
}


}}
