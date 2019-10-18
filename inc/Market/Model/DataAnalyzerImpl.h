#ifndef __ABOLLO_MARKET_MODEL_DATA_ANALYZER_IMPL_H__
#define __ABOLLO_MARKET_MODEL_DATA_ANALYZER_IMPL_H__


#include <string>
#include <tuple>
#include <utility>

#include <date/date.h>
#include <soci/session.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <boost/circular_buffer.hpp>

#include "Market/Model/Price.h"



namespace abollo
{



class DataAnalyzerImpl final
{
private:
    constexpr static const char* INDEX_DAILY_SQL = "SELECT date, open, close, low, high, volume, amount "
                                                   "FROM index_daily_market "
                                                   "WHERE date >= :start AND date <= :end "
                                                   "ORDER BY date DESC";

    constexpr static std::size_t DEFAULT_BUFFER_ROW_SIZE = 6;
    constexpr static std::size_t DEFAULT_BUFFER_COL_SIZE = 1024;
    constexpr static std::size_t DEFAULT_BUFFER_CAPACITY = DEFAULT_BUFFER_ROW_SIZE * DEFAULT_BUFFER_COL_SIZE;

    constexpr static std::size_t DEFAULT_BUFFER_OPEN_POS   = 0;
    constexpr static std::size_t DEFAULT_BUFFER_CLOSE_POS  = DEFAULT_BUFFER_COL_SIZE;
    constexpr static std::size_t DEFAULT_BUFFER_LOW_POS    = DEFAULT_BUFFER_COL_SIZE + DEFAULT_BUFFER_COL_SIZE;
    constexpr static std::size_t DEFAULT_BUFFER_HIGH_POS   = DEFAULT_BUFFER_COL_SIZE * 3;
    constexpr static std::size_t DEFAULT_BUFFER_VOLUME_POS = DEFAULT_BUFFER_COL_SIZE * 4;
    constexpr static std::size_t DEFAULT_BUFFER_AMOUNT_POS = DEFAULT_BUFFER_COL_SIZE * 5;

    soci::session mSession{soci::sqlite3, R"(data/ashare.db)"};

    soci::statement mIndexDailyStmt;

    const std::string mCode;

    /*
     * Price data loaded from database is transferred to gpu and stored as the following format:
     * [0 ~ COL - 1][COL ~ COL * 2 -1][COL * 2 ~ COL * 3 -1][COL * 3 ~ COL * 4 -1][COL * 4 ~ COL * 5 -1][COL * 5 ~ COL * 6 -1]
     *    /\             /\                    /\                    /\                    /\                    /\
     *   open           close                 low                   high                 volume                amount
     */
    thrust::device_vector<float> mDeviceBuffer;
    boost::circular_buffer<date::year_month_day> mDateBuffer{DEFAULT_BUFFER_COL_SIZE};

    mutable thrust::device_vector<Float7> mDeviceTempBuffer{DEFAULT_BUFFER_COL_SIZE};
    mutable thrust::host_vector<Float7> mHostTempBuffer{DEFAULT_BUFFER_COL_SIZE};

    std::size_t mPriceCount{0};

    std::tuple<float, float, float, float, float, float> mMinMax;

public:
    DataAnalyzerImpl() : mIndexDailyStmt(mSession.prepare << INDEX_DAILY_SQL)
    {
    }

    [[nodiscard]] const auto& Buffer() const
    {
        return mDeviceBuffer;
    }

    [[nodiscard]] const auto& Date() const
    {
        return mDateBuffer;
    }

    void LoadIndex(const date::year_month_day& aStartDate, const date::year_month_day& aEndDate);

    [[nodiscard]] std::tuple<float, float, float, float> MinMax(const std::size_t aStartIndex, const std::size_t aSize) const;

    [[nodiscard]] std::pair<DatePriceZipIterator, DatePriceZipIterator> Saxpy(const std::size_t aStartIndex, const std::size_t aSize, const float aScaleX, const float aTransX,
                                                                              const float aScaleY, const float aTransY, const float aScaleZ, const float aTransZ) const;

    [[nodiscard]] std::size_t Size() const
    {
        return mPriceCount;
    }
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_DATA_ANALYZER_IMPL_H__
