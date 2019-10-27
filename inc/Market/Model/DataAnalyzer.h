#ifndef __ABOLLO_MARKET_MODEL_DATA_ANALYZER_H__
#define __ABOLLO_MARKET_MODEL_DATA_ANALYZER_H__



#include <functional>
#include <memory>
#include <string>
#include <tuple>
#include <utility>

#include <date/date.h>

#include "Market/Model/ColumnTraits.h"
#include "Market/Model/Column.h"
#include "Market/Model/PagedMarketingTable.h"
#include "Market/Model/DataLoader.h"



namespace abollo
{



template <typename... Tags>
class DataAnalyzerImpl;



class DataAnalyzer final
{
public:
    using PagedTableType = PagedMarketingTable<float, 10, open_tag, close_tag, low_tag, high_tag, volume_tag, amount_tag>;

private:
    PagedTableType mPagedTable;
    DataLoader mDataLoader;

    std::unique_ptr<DataAnalyzerImpl<PagedTableType::Schema>> mImpl;

public:
    DataAnalyzer();
    ~DataAnalyzer();

    void LoadIndex(const std::string& aCode, const date::year_month_day& aStartDate, const date::year_month_day& aEndDate);

    [[nodiscard]] std::pair<DatePriceZipIterator, DatePriceZipIterator> Saxpy(const std::size_t aStartIndex, const std::size_t aSize, const float aScaleX, const float aTransX,
                                                                              const float aScaleY, const float aTransY, const float aScaleZ, const float aTransZ) const;

    [[nodiscard]] PriceWithIndex operator[](const std::size_t aIndex) const;
    [[nodiscard]] std::size_t Size() const;

    [[nodiscard]] std::tuple<float, float, float, float> MinMax(const std::size_t aStartIndex, const std::size_t aSize) const;
    [[nodiscard]] std::pair<float, float> MinMax(const std::size_t aStartIndex, const std::size_t aSize, const ColumnTraits<price_tag> aTag) const;
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_DATA_ANALYZER_H__
