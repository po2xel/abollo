#ifndef __ABOLLO_MARKET_PRICE_H__
#define __ABOLLO_MARKET_PRICE_H__



#include <date/date.h>
#include <array>

#include <soci/soci.h>
#include <soci/values.h>
#include <thrust/host_vector.h>
#include <thrust/tuple.h>
#include <boost/circular_buffer.hpp>
#include <utility>



namespace abollo
{


using IntFloat6 = thrust::tuple<int, float, float, float, float, float, float>;
using Float7    = thrust::tuple<float, float, float, float, float, float, float>;

using DateIterator         = boost::circular_buffer<date::year_month_day>::const_iterator;
using PriceIterator        = thrust::host_vector<Float7>::const_iterator;
using DatePriceIterator    = thrust::tuple<DateIterator, PriceIterator>;
using DatePriceZipIterator = thrust::zip_iterator<DatePriceIterator>;



enum class DataType
{
    eIndex = 0,

    eOpen  = 0x01,
    eClose = 0x02,
    eLow   = 0x04,
    eHigh  = 0x08,

    eVolume = 0x10,
    eAmount = 0x11,

    ePrice = eOpen | eClose | eLow | eHigh
};



struct Price
{
    date::year_month_day date;

    union {
        std::array<float, 6> data;

        struct
        {
            float open;
            float close;
            float low;
            float high;
            float volume;
            float amount;
        };
    };
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



namespace soci
{



template <>
struct type_conversion<abollo::Price>
{
    using base_type = values;

    static void from_base(const base_type& v, indicator /*ind*/, abollo::Price& price)
    {
        price.date = v.get<date::year_month_day>("date");

        price.open   = static_cast<float>(v.get<double>("open"));
        price.close  = static_cast<float>(v.get<double>("close"));
        price.low    = static_cast<float>(v.get<double>("low"));
        price.high   = static_cast<float>(v.get<double>("high"));
        price.volume = static_cast<float>(v.get<double>("volume") / 1000000.f);
        price.amount = static_cast<float>(v.get<double>("amount") / 1000000.f);
    }
};



}    // namespace soci



#endif    // __ABOLLO_MARKET_PRICE_H__
