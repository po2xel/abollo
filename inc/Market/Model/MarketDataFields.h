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

    __host__ __device__ MarketDataFields(const thrust::tuple<float, float, float, float, float, float, float>& aData)
        : index{aData.get<0>()}, open{aData.get<1>()}, close{aData.get<2>()}, low{aData.get<3>()}, high{aData.get<4>()}, volume{aData.get<5>()}, amount{aData.get<6>()}
    {
    }

    __host__ __device__ auto& operator=(const thrust::tuple<float, float, float, float, float, float, float>& aData)
    {
        index  = aData.get<0>();
        open   = aData.get<1>();
        close  = aData.get<2>();
        low    = aData.get<3>();
        high   = aData.get<4>();
        volume = aData.get<5>();
        amount = aData.get<6>();

        return *this;
    }
};


using DateIterator         = thrust::host_vector<date::year_month_day>::const_iterator;
using PriceIterator        = thrust::host_vector<MarketDataFields>::const_iterator;
using DatePriceIterator    = thrust::tuple<DateIterator, PriceIterator>;
using DatePriceZipIterator = thrust::zip_iterator<DatePriceIterator>;



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_ROW_H__
