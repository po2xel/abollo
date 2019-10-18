#include "Market/Model/DataAnalyzer.h"

#include "Market/Model/DataAnalyzerImpl.h"



namespace abollo
{



DataAnalyzer::DataAnalyzer() : mImpl{std::make_unique<DataAnalyzerImpl>()}
{
}


DataAnalyzer::~DataAnalyzer() = default;


void DataAnalyzer::LoadIndex(const std::string& aCode, const date::year_month_day& aStartDate, const date::year_month_day& aEndDate) const
{
    mImpl->LoadIndex(aCode, aStartDate, aEndDate);
}


std::pair<DatePriceZipIterator, DatePriceZipIterator> DataAnalyzer::Saxpy(const std::size_t aStartIndex, const std::size_t aSize, const float aScaleX, const float aTransX,
                                                                   const float aScaleY, const float aTransY, const float aScaleZ, const float aTransZ) const
{
    return mImpl->Saxpy(aStartIndex, aSize, aScaleX, aTransX, aScaleY, aTransY, aScaleZ, aTransZ);
}


PriceWithIndex DataAnalyzer::operator[](const std::size_t aIndex) const
{
    return (*mImpl)[aIndex];
}


std::size_t DataAnalyzer::Size() const
{
    return mImpl->Size();
}


std::tuple<float, float, float, float> DataAnalyzer::MinMax(const std::size_t aStartIndex, const std::size_t aSize) const
{
    return mImpl->MinMax(aStartIndex, aSize);
}



}    // namespace abollo