#ifndef __ABOLLO_MARKET_MODEL_ROW_H__
#define __ABOLLO_MARKET_MODEL_ROW_H__



#include <date/date.h>
#include <thrust/host_vector.h>
#include <thrust/iterator/zip_iterator.h>
#include <thrust/tuple.h>



namespace abollo
{



struct MarketDataFields
{
    float index{0.f};

    float open{0.f};
    float close{0.f};
    float low{0.f};
    float high{0.f};
    float volume{0.f};
    float amount{0.f};

    __host__ __device__ MarketDataFields()
    {
    }

    __host__ __device__ MarketDataFields(float i, float o, float c, float l, float h, float v, float a) : index{i}, open{o}, close{c}, low{l}, high{h}, volume{v}, amount{a}
    {
    }
};


using DateIterator         = thrust::host_vector<date::year_month_day>::const_iterator;
using PriceIterator        = thrust::host_vector<MarketDataFields>::const_iterator;
using DatePriceIterator    = thrust::tuple<DateIterator, PriceIterator>;
using DatePriceZipIterator = thrust::zip_iterator<DatePriceIterator>;



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_ROW_H__
