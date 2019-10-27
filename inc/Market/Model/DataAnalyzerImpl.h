#ifndef __ABOLLO_MARKET_MODEL_DATA_ANALYZER_IMPL_H__
#define __ABOLLO_MARKET_MODEL_DATA_ANALYZER_IMPL_H__


#include <tuple>
#include <utility>

#include <date/date.h>
#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <boost/circular_buffer.hpp>

#include "Market/Model/CircularMarketingTable.h"
#include "Market/Model/Column.h"
#include "Market/Model/ColumnTraits.h"



namespace abollo
{



template <typename... Tags>
class DataAnalyzerImpl;



template <typename... Tags>
class DataAnalyzerImpl<TableSchema<Tags...>> final
{
private:
    constexpr static std::size_t DEFAULT_BUFFER_ROW_SIZE = 6;
    constexpr static std::size_t DEFAULT_BUFFER_COL_SIZE{1024u};

    constexpr static std::size_t DEFAULT_BUFFER_OPEN_POS   = 0;
    constexpr static std::size_t DEFAULT_BUFFER_CLOSE_POS  = DEFAULT_BUFFER_OPEN_POS + DEFAULT_BUFFER_COL_SIZE;
    constexpr static std::size_t DEFAULT_BUFFER_LOW_POS    = DEFAULT_BUFFER_CLOSE_POS + DEFAULT_BUFFER_COL_SIZE;
    constexpr static std::size_t DEFAULT_BUFFER_HIGH_POS   = DEFAULT_BUFFER_LOW_POS + DEFAULT_BUFFER_COL_SIZE;
    constexpr static std::size_t DEFAULT_BUFFER_VOLUME_POS = DEFAULT_BUFFER_HIGH_POS + DEFAULT_BUFFER_COL_SIZE;
    constexpr static std::size_t DEFAULT_BUFFER_AMOUNT_POS = DEFAULT_BUFFER_VOLUME_POS + DEFAULT_BUFFER_COL_SIZE;
    constexpr static std::size_t DEFAULT_BUFFER_CAPACITY   = DEFAULT_BUFFER_AMOUNT_POS + DEFAULT_BUFFER_COL_SIZE;

    /*
     * Column data loaded from database is transferred to gpu and stored as the following format:
     * [0 ~ COL - 1][COL ~ COL * 2 -1][COL * 2 ~ COL * 3 -1][COL * 3 ~ COL * 4 -1][COL * 4 ~ COL * 5 -1][COL * 5 ~ COL * 6 -1]
     *    /\             /\                    /\                    /\                    /\                    /\
     *   open           close                 low                   high                 volume                amount
     */
    thrust::device_vector<float> mDeviceBuffer;
    boost::circular_buffer<date::year_month_day> mDateBuffer{DEFAULT_BUFFER_COL_SIZE};

    mutable thrust::device_vector<Float7> mDeviceTempBuffer{DEFAULT_BUFFER_COL_SIZE};
    mutable thrust::host_vector<Float7> mHostTempBuffer{DEFAULT_BUFFER_COL_SIZE};

    std::size_t mPriceCount{0};

    CircularMarketingTable<float, 10, Tags...> mMarketingTable;

public:
    DataAnalyzerImpl() : mMarketingTable{""}
    {
    }

    [[nodiscard]] std::tuple<float, float, float, float> MinMax(const std::size_t aStartIndex, const std::size_t aSize) const;
    [[nodiscard]] std::pair<float, float> MinMax(const std::size_t aStartIndex, const std::size_t aSize, const ColumnTraits<price_tag>) const;

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

    template <typename U>
    void Append(U&& aData)
    {
        mMarketingTable.push_back(std::forward<U>(aData));
    }

    template <typename U>
    void Prepend(U&& aData)
    {
        mMarketingTable.push_front(std::forward<U>(aData));
    }
};



}    // namespace abollo



#include "Market/Model/DataAnalyzerImpl.inl"



#endif    // __ABOLLO_MARKET_MODEL_DATA_ANALYZER_IMPL_H__
