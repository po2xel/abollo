#ifndef __ABOLLO_MARKET_PRICE_H__
#define __ABOLLO_MARKET_PRICE_H__



#include <date/date.h>

#include <soci/soci.h>
#include <soci/values.h>

#include <skia/include/core/SkColor.h>



namespace abollo
{



struct Price
{
    date::year_month_day date;

    float open;
    float close;
    float low;
    float high;

    SkColor color;

    Price() = default;

    Price(const float aOpen, const float aClose, const float aLow, const float aHigh, const SkColor aColor) : open(aOpen), close(aClose), low(aLow), high(aHigh), color(aColor)
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

        price.open  = static_cast<float>(v.get<double>("open"));
        price.close = static_cast<float>(v.get<double>("close"));
        price.low   = static_cast<float>(v.get<double>("low"));
        price.high  = static_cast<float>(v.get<double>("high"));

        price.color = price.close > price.open ? SK_ColorRED : SK_ColorGREEN;
    }
};



}    // namespace soci



#endif    // __ABOLLO_MARKET_PRICE_H__
