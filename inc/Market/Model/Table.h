#ifndef __ABOLLO_MARKET_MODEL_TABLE_H__
#define __ABOLLO_MARKET_MODEL_TABLE_H__



#include <algorithm>
#include <string>
#include <utility>

#include <date/date.h>
// #include <thrust/device_vector.h>
#include <thrust/host_vector.h>

#include "Market/Model/ChunkedArray.h"



namespace abollo
{



template <typename C, const std::size_t Size, ColumnType Y>
using Column = ChunkedArray<C, Size, Y>;



template <typename C, const std::size_t Size, ColumnType... Ys>
class Table : protected Column<C, Size, Ys>...
{
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_TABLE_H__
