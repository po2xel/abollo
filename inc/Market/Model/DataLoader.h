#ifndef __ABOLLO_MARKET_MODEL_DATA_LOADER_H__
#define __ABOLLO_MARKET_MODEL_DATA_LOADER_H__



#include <string_view>

#include <date/date.h>
#include <soci/session.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>



namespace abollo
{



class DataLoader
{
private:
    constexpr static const char* INDEX_DAILY_SQL = "SELECT date, open, close, low, high, volume, amount "
                                                        "FROM index_daily_market "
                                                        "WHERE code = :code AND date >= :start AND date <= :end "
                                                        "ORDER BY date DESC";

    soci::session mSession{soci::sqlite3, R"(data/ashare.db)"};

    soci::statement mIndexDailyStmt;

public:
    DataLoader() : mIndexDailyStmt(mSession.prepare << INDEX_DAILY_SQL)
    {
    }

    template <typename T, typename LoadOp>
    void LoadIndex(const std::string& aCode, const date::year_month_day& aStartDate, const date::year_month_day& aEndDate, LoadOp aLoadOp)
    {
        using soci::into;
        using soci::use;

        T lRow{};

        mIndexDailyStmt.exchange(use(aCode, "code"));
        mIndexDailyStmt.exchange(use(aStartDate, "start"));
        mIndexDailyStmt.exchange(use(aEndDate, "end"));
        mIndexDailyStmt.exchange(into(lRow));

        mIndexDailyStmt.define_and_bind();
        mIndexDailyStmt.execute();

        while (mIndexDailyStmt.fetch())
            aLoadOp(lRow);

        mIndexDailyStmt.bind_clean_up();
    }
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_DATA_LOADER_H__
