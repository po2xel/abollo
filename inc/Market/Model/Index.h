#ifndef __ABOLLO_MARKET_INDEX_H__
#define __ABOLLO_MARKET_INDEX_H__



#include <functional>
#include <memory>
#include <string>
#include <tuple>
#include <utility>

#include <date/date.h>

#include "Market/Model/Price.h"



namespace std
{


template <>
struct hash<date::year_month_day>
{
    using argument_type = date::year_month_day;
    using result_type   = std::size_t;

    result_type operator()(const argument_type& s) const noexcept
    {
        return std::hash<int>{}(static_cast<date::sys_days>(s).time_since_epoch().count());
    }
};



}    // namespace std



namespace abollo
{



class IndexImpl;


class Index final
{
private:
    std::unique_ptr<IndexImpl> mImpl;

public:
    Index();
    ~Index();

    void LoadIndex(const date::year_month_day& aStartDate, const date::year_month_day& aEndDate) const;

    [[nodiscard]] std::pair<DatePriceZipIterator, DatePriceZipIterator> Saxpy(const std::size_t aStartIndex, const std::size_t aSize, const float aScaleX, const float aTransX,
                                                                              const float aScaleY, const float aTransY, const float aScaleZ, const float aTransZ) const;

    [[nodiscard]] std::size_t Size() const;

    [[nodiscard]] std::tuple<float, float, float, float> MinMax(const std::size_t aStartIndex, const std::size_t aSize) const;
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_INDEX_PAINTER_H__
