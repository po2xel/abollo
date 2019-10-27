#ifndef __ABOLLO_MARKET_MODEL_DATA_ANALYZER_H__
#define __ABOLLO_MARKET_MODEL_DATA_ANALYZER_H__



#include <memory>
#include <string>
#include <utility>

#include <date/date.h>
#include <thrust/uninitialized_copy.h>

#include "Market/Model/ColumnTraits.h"
#include "Market/Model/DataLoader.h"
#include "Market/Model/MarketDataFields.h"
#include "Market/Model/PagedMarketingTable.h"



namespace abollo
{



template <const uint8_t P, typename F, typename Tags>
class DataAnalyzerImpl;



class DataAnalyzer final
{
private:
    constexpr static uint8_t DEFAULT_BUFFER_COL_POWER    = 10;
    constexpr static std::size_t DEFAULT_BUFFER_COL_SIZE = 1 << DEFAULT_BUFFER_COL_POWER;

public:
    using PagedTableType = PagedMarketingTable<float, DEFAULT_BUFFER_COL_POWER, open_tag, close_tag, low_tag, high_tag, volume_tag, amount_tag>;
    using DataSchema     = PagedTableType::Schema;
    using ImplType       = DataAnalyzerImpl<DEFAULT_BUFFER_COL_POWER, MarketDataFields, PagedTableType::Schema>;

private:
    class Cache
    {
    private:
        std::size_t mOffset{0};

        thrust::host_vector<MarketDataFields> mBuffer;

    public:
        template <typename Iterator>
        void Assign(Iterator aBegin, Iterator aEnd, const std::size_t aOffset)
        {
            mBuffer.assign(aBegin, aEnd);
            mOffset = aOffset;
        }

        [[nodiscard]] bool Contains(const std::size_t aIndex) const
        {
            return mOffset <= aIndex && aIndex < (mOffset + mBuffer.size());
        }

        [[nodiscard]] auto& operator[](const std::size_t aIndex) const
        {
            assert(Contains(aIndex));

            return mBuffer[aIndex - mOffset];
        }
    };

    PagedTableType mPagedTable;
    DataLoader mDataLoader;
    mutable Cache mMarketingDataCache;

    std::unique_ptr<ImplType> mImpl;

public:
    DataAnalyzer();
    ~DataAnalyzer();

    void LoadIndex(const std::string& aCode, const date::year_month_day& aStartDate, const date::year_month_day& aEndDate);

    [[nodiscard]] std::pair<DatePriceZipIterator, DatePriceZipIterator> Saxpy(const std::size_t aStartIndex, const std::size_t aSize, const float aScaleX, const float aTransX,
                                                                              const float aScaleY, const float aTransY, const float aScaleZ, const float aTransZ) const;

    [[nodiscard]] std::pair<DatePriceZipIterator, DatePriceZipIterator> LogSaxpy(const std::size_t aStartIndex, const std::size_t aSize, const float aScaleX, const float aTransX,
                                                                                 const float aScaleY, const float aTransY, const float aScaleZ, const float aTransZ) const;

    [[nodiscard]] MarketDataFields operator[](const std::size_t aIndex) const;
    [[nodiscard]] MarketDataFields Get(const std::size_t aIndex) const;
    [[nodiscard]] std::size_t Size() const;

    [[nodiscard]] std::pair<float, float> MinMax(const std::size_t aStartIndex, const std::size_t aSize, ColumnTraits<price_tag>) const;
    [[nodiscard]] std::pair<float, float> MinMax(const std::size_t aStartIndex, const std::size_t aSize, ColumnTraits<log_price_tag>) const;

    [[nodiscard]] std::pair<float, float> MinMax(const std::size_t aStartIndex, const std::size_t aSize, ColumnTraits<volume_tag>) const;
    [[nodiscard]] std::pair<float, float> MinMax(const std::size_t aStartIndex, const std::size_t aSize, ColumnTraits<log_volume_tag>) const;
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_DATA_ANALYZER_H__
