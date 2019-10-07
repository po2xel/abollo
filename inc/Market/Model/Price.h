#ifndef __ABOLLO_MARKET_PRICE_H__
#define __ABOLLO_MARKET_PRICE_H__



#include <date/date.h>

#include <soci/soci.h>
#include <soci/values.h>



namespace abollo
{



enum class Data
{
    ePrice,
    eVolume,
    eAmount
};



struct Price
{
    date::year_month_day date;

    float open;
    float close;
    float low;
    float high;
    float volume;
    float amount;
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
        price.volume = static_cast<float>(v.get<double>("volume"));
        price.amount = static_cast<float>(v.get<double>("amount"));
    }
};



}    // namespace soci



#endif    // __ABOLLO_MARKET_PRICE_H__
