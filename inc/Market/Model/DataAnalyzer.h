#ifndef __ABOLLO_MARKET_MODEL_DATA_ANALYZER_H__
#define __ABOLLO_MARKET_MODEL_DATA_ANALYZER_H__



#include <memory>
#include <string>
#include <utility>

#include <date/date.h>

#include "Market/Model/ColumnTraits.h"
#include "Market/Model/DataLoader.h"
#include "Market/Model/MarketDataFields.h"
#include "Market/Model/PagedMarketingTable.h"



namespace abollo
{



template <const uint8_t P, typename T, typename S>
class DataAnalyzerImpl;



class DataAnalyzer final
{
private:
    constexpr static uint8_t DEFAULT_BUFFER_COL_POWER = 10;
    constexpr static uint32_t DEFAULT_BUFFER_COL_SIZE = 1 << DEFAULT_BUFFER_COL_POWER;

public:
    using DataSchema     = TableSchema<date_tag, seq_tag, open_tag, close_tag, low_tag, high_tag, volume_tag, amount_tag>;
    using PagedTableType = PagedMarketingTable<float, DEFAULT_BUFFER_COL_POWER, DataSchema>;
    using ImplType       = DataAnalyzerImpl<DEFAULT_BUFFER_COL_POWER, MarketDataFields, DataSchema>;

private:
    DataLoader mDataLoader;
    uint32_t mStartSeq{0};
    uint32_t mEndSeq{0};

    std::unique_ptr<ImplType> mImpl;

    [[nodiscard]] std::pair<uint32_t, uint32_t> Normalize(uint32_t aStartIndex, uint32_t aEndIndex) const
    {
        if (mStartSeq > aStartIndex)
            aStartIndex = mStartSeq;

        if (mEndSeq < aEndIndex)
            aEndIndex = mEndSeq;

        assert(aEndIndex > aStartIndex);

        return {mEndSeq - aEndIndex, mEndSeq - aStartIndex + 1u};
    }

public:
    DataAnalyzer();
    ~DataAnalyzer();

    std::pair<std::uint32_t, std::uint32_t> LoadIndex(const std::string& aCode, const uint32_t& aOffset, const uint32_t& aLimit);

    [[nodiscard]] MarketDataFields operator[](const uint32_t aIndex) const;
    [[nodiscard]] uint32_t Size() const;

    template <typename T>
    [[nodiscard]] std::pair<float, float> Range() const;

    template <typename T>
    [[nodiscard]] std::pair<float, float> MinMax(const uint32_t aStartIndex, const uint32_t aEndIndex) const;

    template <typename T>
    [[nodiscard]] std::pair<DatePriceZipIterator, DatePriceZipIterator> Saxpy(const uint32_t aStartIndex, const uint32_t aEndIndex, const float aScaleX, const float aTransX,
                                                                              const float aScaleY, const float aTransY, const float aScaleZ, const float aTransZ) const;
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_DATA_ANALYZER_H__
