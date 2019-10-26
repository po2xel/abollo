#ifndef __ABOLLO_MARKET_PRICE_H__
#define __ABOLLO_MARKET_PRICE_H__



#include <tuple>

#include <date/date.h>
#include <thrust/host_vector.h>
#include <thrust/tuple.h>
#include <utility>

#include "Market/Model/ColumnTraits.h"



namespace abollo
{


using IntFloat6 = thrust::tuple<int, float, float, float, float, float, float>;
using Float7    = thrust::tuple<float, float, float, float, float, float, float>;

using DateIterator         = thrust::host_vector<date::year_month_day>::const_iterator;
using PriceIterator        = thrust::host_vector<Float7>::const_iterator;
using DatePriceIterator    = thrust::tuple<DateIterator, PriceIterator>;
using DatePriceZipIterator = thrust::zip_iterator<DatePriceIterator>;



template <typename Tag>
struct Price
{
    using type = Tag;

    float value;
};


template <>
struct Price<date_tag>
{
    using type = date_tag;

    date::year_month_day value;
};


template <typename... T>
struct Prices;


template <typename... Tags>
struct Prices<TableSchema<Tags...>> : Price<Tags>...
{
    template <typename T, typename V>
    auto& Set(V&& aValue)
    {
        Price<T>::value = std::forward<V>(aValue);

        return *this;
    }

    template <typename T>
    [[nodiscard]] auto Get() const
    {
        return Price<T>::value;
    }

    template <typename... Ts, typename = std::enable_if_t<(sizeof...(Ts) > 1u)>>
    [[nodiscard]] auto Get() const
    {
        return std::make_tuple(Price<Ts>::value...);
    }
};



union PriceWithIndex {
    Float7 tuples{};

    struct
    {
        float index;

        float open;
        float close;
        float low;
        float high;
        float volume;
        float amount;
    };

    explicit PriceWithIndex(Float7 aFloat7) : tuples(std::move(aFloat7))
    {
    }
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_PRICE_H__
