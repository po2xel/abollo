#ifndef __ABOLLO_MARKET_MODEL_TABLE_H__
#define __ABOLLO_MARKET_MODEL_TABLE_H__



#include "Market/Model/ChunkedArray.h"



namespace abollo
{



template <typename C, const std::size_t Size, typename Y>
using Column = ChunkedArray<C, Size, Y>;



template <typename C, const std::size_t Size, typename... Ys>
class Table : protected Column<C, Size, Ys>...
{
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_TABLE_H__
