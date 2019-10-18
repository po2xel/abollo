#include "Market/Model/Index.h"

#include "Market/Model/IndexImpl.h"



namespace abollo
{



Index::Index() : mImpl{std::make_unique<IndexImpl>()}
{
}


Index::~Index() = default;


void Index::LoadIndex(const date::year_month_day& aStartDate, const date::year_month_day& aEndDate) const
{
    mImpl->LoadIndex(aStartDate, aEndDate);
}


std::pair<DatePriceZipIterator, DatePriceZipIterator> Index::Saxpy(const std::size_t aStartIndex, const std::size_t aSize, const float aScaleX, const float aTransX,
                                                                   const float aScaleY, const float aTransY, const float aScaleZ, const float aTransZ) const
{
    return mImpl->Saxpy(aStartIndex, aSize, aScaleX, aTransX, aScaleY, aTransY, aScaleZ, aTransZ);
}

std::size_t Index::Size() const
{
    return mImpl->Size();
}


std::tuple<float, float, float, float> Index::MinMax(const std::size_t aStartIndex, const std::size_t aSize) const
{
    return mImpl->MinMax(aStartIndex, aSize);
}



}    // namespace abollo