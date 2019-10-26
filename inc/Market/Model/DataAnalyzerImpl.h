#ifndef __ABOLLO_MARKET_MODEL_DATA_ANALYZER_IMPL_H__
#define __ABOLLO_MARKET_MODEL_DATA_ANALYZER_IMPL_H__


#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include <date/date.h>
#include <soci/session.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <boost/circular_buffer.hpp>

#include "Market/Model/Price.h"
#include "Market/Model/Table.h"



namespace abollo
{



template <typename T, const std::size_t P, ColumnType Y>
using HostColumn = Column<thrust::host_vector<T>, P, Y>;


template <typename T, const std::size_t Cap, ColumnType Y>
using DeviceColumn = Column<thrust::device_vector<T>, Cap, Y>;



template <typename T, const std::size_t P, ColumnType... Ys>
class MarketingTable : private Column<thrust::host_vector<date::year_month_day>, 1 << P, ColumnType::eDate>, public Table<thrust::device_vector<T>, 1 << P, Ys...>
{
private:
    constexpr static std::size_t CAPACITY      = 1 << P;
    constexpr static std::size_t CAPACITY_MASK = CAPACITY - 1;

    std::string mCode;

    std::size_t mFirst{0};
    std::size_t mLast{0};
    std::size_t mSize{0};

    [[nodiscard]] auto SpaceLeft() const
    {
        return mFirst <= mLast ? CAPACITY + mFirst - mLast : mFirst - mLast;
    }

    [[nodiscard]] auto Full() const
    {
        return CAPACITY == mSize;
    }

    void Forward(const std::size_t aSize)
    {
        if (mLast + aSize >= CAPACITY_MASK)
            mLast = (mLast + aSize) & CAPACITY_MASK;
        else
            mLast += aSize;

        mSize = std::min(mSize + aSize, CAPACITY);

        if (Full())
            mFirst = mLast;
    }

    void Backward(const std::size_t aSize)
    {
        if (mFirst >= aSize)
            mFirst -= aSize;
        else
            mFirst = (mFirst + CAPACITY - aSize) & CAPACITY_MASK;

        mSize = std::min(mSize + aSize, CAPACITY);

        if (Full())
            mLast = mFirst;
    }

public:
    explicit MarketingTable(std::string aCode) noexcept : mCode{std::move(aCode)}
    {
    }

    template <ColumnType Y>
    auto begin() const
    {
        return DeviceColumn<T, CAPACITY, Y>::begin();
    }

    template <ColumnType Y>
    auto end() const
    {
        return DeviceColumn<T, CAPACITY, Y>::end();
    }

    template <ColumnType Y, typename Iterator>
    void Append(Iterator aBegin, Iterator aEnd)
    {
        const auto lDistance = std::distance(aBegin, aEnd);
        assert(lDistance >= 0 && static_cast<std::size_t>(lDistance) <= CAPACITY);

        using BaseType = std::conditional_t<ColumnType::eDate == Y, HostColumn<T, CAPACITY, Y>, DeviceColumn<T, CAPACITY, Y>>;

        const auto lSize = std::min(CAPACITY - mLast, static_cast<std::size_t>(lDistance));
        thrust::copy_n(aBegin, lSize, BaseType::begin() + mLast);

        const auto lLeft = lDistance - lSize;

        if (lLeft > 0)
            thrust::copy_n(aBegin + lSize, lLeft, BaseType::begin());

        Forward(lDistance);
    }

    template <ColumnType Y, typename Iterator>
    void Prepend(Iterator aBegin, Iterator aEnd)
    {
        const auto lDistance = std::distance(aBegin, aEnd);
        assert(lDistance >= 0 && static_cast<std::size_t>(lDistance) <= CAPACITY);

        using BaseType = std::conditional_t<ColumnType::eDate == Y, HostColumn<T, CAPACITY, Y>, DeviceColumn<T, CAPACITY, Y>>;

        const auto lSize = std::min(mFirst, static_cast<std::size_t>(lDistance));
        thrust::copy_n(aEnd - lSize, lSize, BaseType::begin() + mFirst - lSize);

        const auto lLeft = lDistance - lSize;

        if (lLeft > 0)
            thrust::copy_n(aBegin, lLeft, BaseType::end() - lLeft);

        Backward(lDistance);
    }
};



class DataAnalyzerImpl final
{
private:
    constexpr static const char* INDEX_DAILY_SQL = "SELECT date, open, close, low, high, volume, amount "
                                                   "FROM index_daily_market "
                                                   "WHERE code = :code AND date >= :start AND date <= :end "
                                                   "ORDER BY date DESC";

    constexpr static std::size_t DEFAULT_BUFFER_ROW_SIZE = 6;
    constexpr static std::size_t DEFAULT_BUFFER_COL_SIZE{10240u};

    constexpr static std::size_t DEFAULT_BUFFER_OPEN_POS   = 0;
    constexpr static std::size_t DEFAULT_BUFFER_CLOSE_POS  = DEFAULT_BUFFER_OPEN_POS + DEFAULT_BUFFER_COL_SIZE;
    constexpr static std::size_t DEFAULT_BUFFER_LOW_POS    = DEFAULT_BUFFER_CLOSE_POS + DEFAULT_BUFFER_COL_SIZE;
    constexpr static std::size_t DEFAULT_BUFFER_HIGH_POS   = DEFAULT_BUFFER_LOW_POS + DEFAULT_BUFFER_COL_SIZE;
    constexpr static std::size_t DEFAULT_BUFFER_VOLUME_POS = DEFAULT_BUFFER_HIGH_POS + DEFAULT_BUFFER_COL_SIZE;
    constexpr static std::size_t DEFAULT_BUFFER_AMOUNT_POS = DEFAULT_BUFFER_VOLUME_POS + DEFAULT_BUFFER_COL_SIZE;
    constexpr static std::size_t DEFAULT_BUFFER_CAPACITY   = DEFAULT_BUFFER_AMOUNT_POS + DEFAULT_BUFFER_COL_SIZE;

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

    MarketingTable<float, 3, ColumnType::eOpen, ColumnType::eClose> mMarketingTable;

public:
    DataAnalyzerImpl() : mIndexDailyStmt(mSession.prepare << INDEX_DAILY_SQL), mMarketingTable{""}
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

    void LoadIndex(const std::string& aCode, const date::year_month_day& aStartDate, const date::year_month_day& aEndDate);

    [[nodiscard]] std::tuple<float, float, float, float> MinMax(const std::size_t aStartIndex, const std::size_t aSize) const;

    [[nodiscard]] std::pair<DatePriceZipIterator, DatePriceZipIterator> Saxpy(const std::size_t aStartIndex, const std::size_t aSize, const float aScaleX, const float aTransX,
                                                                              const float aScaleY, const float aTransY, const float aScaleZ, const float aTransZ) const;

    [[nodiscard]] PriceWithIndex operator[](const std::size_t aIndex) const
    {
        return PriceWithIndex(mHostTempBuffer[aIndex]);
    }

    [[nodiscard]] std::size_t Size() const
    {
        return mPriceCount;
    }
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_DATA_ANALYZER_IMPL_H__
