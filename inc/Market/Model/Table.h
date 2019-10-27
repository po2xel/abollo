#ifndef __ABOLLO_MARKET_MODEL_TABLE_H__
#define __ABOLLO_MARKET_MODEL_TABLE_H__



#include "Market/Model/ChunkedArray.h"
#include "Market/Model/ColumnTraits.h"



namespace abollo
{



template <typename C, const std::size_t Size, typename Y>
using Column = ChunkedArray<C, Size, Y>;



template <typename Tag>
struct RowValue
{
    using type = Tag;

    float value;
};


template <>
struct RowValue<date_tag>
{
    using type = date_tag;

    date::year_month_day value;
};


template <typename... T>
struct Row;


template <typename... Tags>
struct Row<TableSchema<Tags...>> : RowValue<Tags>...
{
    template <typename T, typename V>
    auto& Set(V&& aValue)
    {
        RowValue<T>::value = std::forward<V>(aValue);

        return *this;
    }

    template <typename T>
    [[nodiscard]] auto Get() const
    {
        return RowValue<T>::value;
    }

    template <typename... Ts, typename = std::enable_if_t<(sizeof...(Ts) > 1u)>>
    [[nodiscard]] auto Get() const
    {
        return std::make_tuple(RowValue<Ts>::value...);
    }
};



template <typename C, const std::size_t Size, typename... Ys>
class Table : protected Column<C, Size, Ys>...
{
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_TABLE_H__
