#ifndef __ABOLLO_MARKET_INDEX_H__
#define __ABOLLO_MARKET_INDEX_H__



#include <functional>
#include <string_view>

#include <boost/circular_buffer.hpp>

#include <date/date.h>

#include "Market/Model/Price.h"
#include "Market/Model/TradeDate.h"



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



class Index final
{
private:
    constexpr static std::string_view INDEX_DAILY_SQL = "SELECT date, open, close, low, high "
                                                        "FROM index_daily_market "
                                                        "WHERE date >= :start AND date <= :end "
                                                        "ORDER BY date ASC";

    constexpr static std::string_view MIN_MAX_SQL = "SELECT min(low), max(high) "
                                                    "FROM index_daily_market "
                                                    "WHERE date >= :start AND date <= :end ";

    soci::session mSession{soci::sqlite3, R"(data/ashare.db)"};
    soci::statement mIndexDailyStmt;
    soci::statement mMinMaxStmt;

    boost::circular_buffer_space_optimized<Price> mPrices{{1024, 20}};
    std::pair<float, float> mMinMax;

    void LoadPrices(const date::year_month_day& aStartDate, const date::year_month_day& aEndDate);
    void LoadMinMax(const date::year_month_day& aStartDate, const date::year_month_day& aEndDate);

public:
    Index() : mIndexDailyStmt(mSession.prepare << INDEX_DAILY_SQL), mMinMaxStmt(mSession.prepare << MIN_MAX_SQL)
    {
    }

    void Load(const date::year_month_day& aStartDate, const date::year_month_day& aEndDate)
    {
        LoadPrices(aStartDate, aEndDate);
        LoadMinMax(aStartDate, aEndDate);
    }

    [[nodiscard]] auto& MinMax() const
    {
        return mMinMax;
    }

    auto& operator[](const std::size_t aIndex) const
    {
        return mPrices.at(aIndex);
    }

    [[nodiscard]] std::size_t size() const
    {
        return mPrices.size();
    }

    [[nodiscard]] auto begin() const
    {
        return mPrices.begin();
    }

    [[nodiscard]] auto end() const
    {
        return mPrices.end();
    }
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_INDEX_PAINTER_H__
