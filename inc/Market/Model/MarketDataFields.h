#ifndef __ABOLLO_MARKET_MODEL_ROW_H__
#define __ABOLLO_MARKET_MODEL_ROW_H__



#include <date/date.h>
#include <thrust/host_vector.h>
#include <thrust/iterator/zip_iterator.h>
#include <thrust/tuple.h>



namespace abollo
{


template <typename I, typename V>
struct MarketDataField
{
    using index_type = I;
    using value_type = V;
    using tuple_type = thrust::tuple<index_type, value_type, value_type, value_type, value_type, value_type, value_type>;

    index_type seq{};

    value_type open{};
    value_type close{};
    value_type low{};
    value_type high{};
    value_type volume{};
    value_type amount{};

    __host__ __device__ MarketDataField()
    {
    }

    __host__ __device__ MarketDataField(index_type i, value_type o, value_type c, value_type l, value_type h, value_type v, value_type a)
        : seq{i}, open{o}, close{c}, low{l}, high{h}, volume{v}, amount{a}
    {
    }

    __host__ __device__ auto& operator=(const tuple_type& aData)
    {
        seq    = thrust::get<0>(aData);
        open   = thrust::get<1>(aData);
        close  = thrust::get<2>(aData);
        low    = thrust::get<3>(aData);
        high   = thrust::get<4>(aData);
        volume = thrust::get<5>(aData);
        amount = thrust::get<6>(aData);

        return *this;
    }
};



struct MarketDataFields
{
    MarketDataField<float, float> original;
    MarketDataField<float, float> transformed;

    __host__ __device__ MarketDataFields()
    {
    }

    __host__ __device__ MarketDataFields(const MarketDataField<float, float>& aOriginal, const MarketDataField<float, float>& aTransformed)
        : original{aOriginal}, transformed{aTransformed}
    {
    }
};


using DateIterator         = thrust::host_vector<date::year_month_day>::const_iterator;
using PriceIterator        = thrust::host_vector<MarketDataFields>::const_iterator;
using DatePriceIterator    = thrust::tuple<DateIterator, PriceIterator>;
using DatePriceZipIterator = thrust::zip_iterator<DatePriceIterator>;
using DatePricePair        = std::pair<DatePriceZipIterator, DatePriceZipIterator>;



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_ROW_H__
