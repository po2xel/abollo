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
    constexpr static uint8_t DEFAULT_BUFFER_COL_POWER    = 10;
    constexpr static std::size_t DEFAULT_BUFFER_COL_SIZE = 1 << DEFAULT_BUFFER_COL_POWER;

public:
    using DataSchema     = TableSchema<date_tag, open_tag, close_tag, low_tag, high_tag, volume_tag, amount_tag>;
    using PagedTableType = PagedMarketingTable<float, DEFAULT_BUFFER_COL_POWER, DataSchema>;
    using ImplType       = DataAnalyzerImpl<DEFAULT_BUFFER_COL_POWER, MarketDataFields, DataSchema>;

private:
    PagedTableType mPagedTable;
    DataLoader mDataLoader;

    std::unique_ptr<ImplType> mImpl;

public:
    DataAnalyzer();
    ~DataAnalyzer();

    void LoadIndex(const std::string& aCode, const date::year_month_day& aStartDate, const date::year_month_day& aEndDate);

    [[nodiscard]] MarketDataFields operator[](const std::size_t aIndex) const;
    [[nodiscard]] std::size_t Size() const;

    template <typename T>
    [[nodiscard]] std::pair<float, float> MinMax(const std::size_t aStartIndex, const std::size_t aSize) const;

    template <typename T>
    [[nodiscard]] std::pair<DatePriceZipIterator, DatePriceZipIterator> Saxpy(const std::size_t aStartIndex, const std::size_t aSize, const float aScaleX, const float aTransX,
                                                                              const float aScaleY, const float aTransY, const float aScaleZ, const float aTransZ) const;
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_DATA_ANALYZER_H__
