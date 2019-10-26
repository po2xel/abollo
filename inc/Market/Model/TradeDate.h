#ifndef __ABOLLO_MARKET_TRADE_DATE_H__
#define __ABOLLO_MARKET_TRADE_DATE_H__



#include <string_view>
#include <vector>

#include <date/date.h>

#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>



namespace abollo
{



class TradeDate final
{
private:
    const std::string EX_SSE = "SSE";

    const std::string TRADE_DATE_SQL = "SELECT date "
                                       "FROM trade_calendar "
                                       "WHERE exchange = :exchange AND is_open = 1 "
                                       "ORDER BY date DESC "
                                       "LIMIT 20";

    soci::session mSession{soci::sqlite3, R"(data/ashare.db)"};
    soci::statement mTradeDateStmt;

    std::vector<date::year_month_day> mTradeDates{20};

public:
    TradeDate() : mTradeDateStmt(mSession.prepare << TRADE_DATE_SQL)
    {
    }

    void Load()
    {
        // mTradeDateStmt.exchange(soci::use(EX_SSE, "exchange"));
        // mTradeDateStmt.exchange(soci::into(mTradeDates));
        //
        // mTradeDateStmt.define_and_bind();
        // mTradeDateStmt.execute(true);
        // mTradeDateStmt.bind_clean_up();
    }

    auto& operator[](const std::size_t aIndex) const
    {
        return mTradeDates.at(aIndex);
    }

    [[nodiscard]] std::size_t size() const
    {
        return mTradeDates.size();
    }

    [[nodiscard]] auto& front() const
    {
        return mTradeDates.back();
    }

    [[nodiscard]] auto& back() const
    {
        return mTradeDates.front();
    }

    [[nodiscard]] auto begin() const
    {
        return mTradeDates.cbegin();
    }

    [[nodiscard]] auto end() const
    {
        return mTradeDates.cend();
    }
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_TRADE_DATE_H__
