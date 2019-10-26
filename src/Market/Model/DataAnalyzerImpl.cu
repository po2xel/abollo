#include "Market/Model/DataAnalyzerImpl.h"

#include <thrust/extrema.h>
#include <thrust/functional.h>
#include <thrust/host_vector.h>
#include <thrust/iterator/zip_iterator.h>
#include <thrust/transform.h>
#include <thrust/tuple.h>

#include "Market/Model/TradeDate.h"



namespace abollo
{



void DataAnalyzerImpl::LoadIndex(const std::string& /*aCode*/, const date::year_month_day& /*aStartDate*/, const date::year_month_day& /*aEndDate*/)
{
    // using soci::into;
    // using soci::use;

    // Price lPrice{};

    // mIndexDailyStmt.exchange(use(aCode, "code"));
    // mIndexDailyStmt.exchange(use(aStartDate, "start"));
    // mIndexDailyStmt.exchange(use(aEndDate, "end"));
    // mIndexDailyStmt.exchange(into(lPrice));

    // mIndexDailyStmt.define_and_bind();
    // mIndexDailyStmt.execute();

    // thrust::host_vector<float> lPriceData{DEFAULT_BUFFER_CAPACITY};

    // PagedMarketingTable<float, 10, low_tag, high_tag> lPagedTable;

    // while (mIndexDailyStmt.fetch())
    //{
    //    mDateBuffer.push_back(lPrice.date);

    //    lPriceData[mPriceCount]                               = lPrice.open;
    //    lPriceData[mPriceCount + DEFAULT_BUFFER_COL_SIZE]     = lPrice.close;
    //    lPriceData[mPriceCount + DEFAULT_BUFFER_COL_SIZE * 2] = lPrice.low;
    //    lPriceData[mPriceCount + DEFAULT_BUFFER_COL_SIZE * 3] = lPrice.high;
    //    lPriceData[mPriceCount + DEFAULT_BUFFER_COL_SIZE * 4] = lPrice.volume;
    //    lPriceData[mPriceCount + DEFAULT_BUFFER_COL_SIZE * 5] = lPrice.amount;

    //    // lPagedTable.push_back<low_tag>(lPrice.low);
    //    // lPagedTable.push_back<high_tag>(lPrice.high);

    //    lPagedTable.push_back(lPrice);

    //    ++mPriceCount;
    //}

    // mIndexDailyStmt.bind_clean_up();

    // std::copy(lPagedTable.begin<low_tag>(), lPagedTable.begin<low_tag>() + 10, std::ostream_iterator<float>(std::cout, "\t"));
    // std::cout << std::endl;

    // std::copy(lPagedTable.begin<high_tag>(), lPagedTable.begin<high_tag>() + 10, std::ostream_iterator<float>(std::cout, "\t"));
    // std::cout << std::endl;

    // mMarketingTable.Append<date_tag>(mDateBuffer.begin(), mDateBuffer.end());
    // mMarketingTable.Append<low_tag>(lPagedTable.begin<low_tag>(), lPagedTable.end<low_tag>());
    // mMarketingTable.Append<high_tag>(lPagedTable.begin<high_tag>(), lPagedTable.end<high_tag>());

    // std::copy(mMarketingTable.begin<date_tag>(), mMarketingTable.begin<date_tag>() + 10, std::ostream_iterator<date::year_month_day>(std::cout, "\t"));
    // std::cout << std::endl;

    // std::copy(mMarketingTable.begin<low_tag>(), mMarketingTable.begin<low_tag>() + 10, std::ostream_iterator<float>(std::cout, "\t"));
    // std::cout << std::endl;

    // std::copy(mMarketingTable.begin<high_tag>(), mMarketingTable.begin<high_tag>() + 10, std::ostream_iterator<float>(std::cout, "\t"));

    // mDeviceBuffer = lPriceData;
}


std::tuple<float, float, float, float> DataAnalyzerImpl::MinMax(const std::size_t aStartIndex, const std::size_t aSize) const
{
    if (aStartIndex > mPriceCount)
        throw std::out_of_range("Start index out of range.");

    const auto lEndIndex = aStartIndex + std::min(aSize, mPriceCount - aStartIndex);

    const auto lMinIter = thrust::min_element(mDeviceBuffer.begin() + DEFAULT_BUFFER_LOW_POS + aStartIndex, mDeviceBuffer.begin() + DEFAULT_BUFFER_LOW_POS + lEndIndex);
    const auto lMaxIter = thrust::max_element(mDeviceBuffer.begin() + DEFAULT_BUFFER_HIGH_POS + aStartIndex, mDeviceBuffer.begin() + DEFAULT_BUFFER_HIGH_POS + lEndIndex);

    const auto lVolMinMaxIter =
        thrust::minmax_element(mDeviceBuffer.begin() + DEFAULT_BUFFER_VOLUME_POS + aStartIndex, mDeviceBuffer.begin() + DEFAULT_BUFFER_VOLUME_POS + lEndIndex);

    return std::make_tuple(*lMinIter, *lMaxIter, *lVolMinMaxIter.first, *lVolMinMaxIter.second);
}


std::pair<float, float> DataAnalyzerImpl::MinMax(const std::size_t aStartIndex, const std::size_t aSize, const ColumnTraits<price_tag>) const
{
    if (aStartIndex > mPriceCount)
        throw std::out_of_range("Start index out of range.");

    const auto lEndIndex = aStartIndex + std::min(aSize, mPriceCount - aStartIndex);

    const auto lMinIter = thrust::min_element(mMarketingTable.begin<low_tag>() + aStartIndex, mMarketingTable.begin<low_tag>() + lEndIndex);
    const auto lMaxIter = thrust::min_element(mMarketingTable.begin<high_tag>() + aStartIndex, mMarketingTable.begin<high_tag>() + lEndIndex);

    return std::make_pair(*lMinIter, *lMaxIter);
}


std::pair<DatePriceZipIterator, DatePriceZipIterator> DataAnalyzerImpl::Saxpy(const std::size_t aStartIndex, const std::size_t aSize, const float aScaleX, const float aTransX,
                                                                              const float aScaleY, const float aTransY, const float aScaleZ, const float aTransZ) const
{
    using thrust::placeholders::_1;

    if (aStartIndex > mPriceCount)
        throw std::out_of_range("Start index out of range.");

    const auto lSize     = std::min(aSize, mPriceCount - aStartIndex);
    const auto lEndIndex = aStartIndex + lSize;

    const thrust::counting_iterator<int> lCounterBegin{static_cast<int>(aStartIndex)};
    const thrust::counting_iterator<int> lCounterEnd{static_cast<int>(aStartIndex + lSize)};

    const auto lBeginIter = thrust::make_zip_iterator(
        make_tuple(lCounterBegin, mDeviceBuffer.begin() + DEFAULT_BUFFER_OPEN_POS + aStartIndex, mDeviceBuffer.begin() + DEFAULT_BUFFER_CLOSE_POS + aStartIndex,
                   mDeviceBuffer.begin() + DEFAULT_BUFFER_LOW_POS + aStartIndex, mDeviceBuffer.begin() + DEFAULT_BUFFER_HIGH_POS + aStartIndex,
                   mDeviceBuffer.begin() + DEFAULT_BUFFER_VOLUME_POS + aStartIndex, mDeviceBuffer.begin() + DEFAULT_BUFFER_AMOUNT_POS + aStartIndex));

    const auto lEndIter =
        thrust::make_zip_iterator(make_tuple(lCounterEnd, mDeviceBuffer.begin() + DEFAULT_BUFFER_OPEN_POS + lEndIndex, mDeviceBuffer.begin() + DEFAULT_BUFFER_CLOSE_POS + lEndIndex,
                                             mDeviceBuffer.begin() + DEFAULT_BUFFER_LOW_POS + lEndIndex, mDeviceBuffer.begin() + DEFAULT_BUFFER_HIGH_POS + lEndIndex,
                                             mDeviceBuffer.begin() + DEFAULT_BUFFER_VOLUME_POS + lEndIndex, mDeviceBuffer.begin() + DEFAULT_BUFFER_AMOUNT_POS + lEndIndex));

    mDeviceTempBuffer.clear();

    thrust::transform(lBeginIter, lEndIter, mDeviceTempBuffer.begin(),
                      [sx = aScaleX, tx = aTransX, sy = aScaleY, ty = aTransY, sz = aScaleZ, tz = aTransZ] __device__(const IntFloat6& a) {
                          return thrust::make_tuple(a.get<0>() * sx + tx, a.get<1>() * sy + ty, a.get<2>() * sy + ty, a.get<3>() * sy + ty, a.get<4>() * sy + ty,
                                                    a.get<5>() * sz + tz, a.get<6>() * sz + tz);
                      });

    mHostTempBuffer.assign(mDeviceTempBuffer.begin(), mDeviceTempBuffer.begin() + lSize);

    // return std::make_pair(thrust::make_zip_iterator(thrust::make_tuple(mDateBuffer.begin() + aStartIndex, mHostTempBuffer.begin())),
    //                       thrust::make_zip_iterator(thrust::make_tuple(mDateBuffer.begin() + lEndIndex, mHostTempBuffer.end())));

    return std::make_pair(thrust::make_zip_iterator(thrust::make_tuple(mMarketingTable.begin<date_tag>() + aStartIndex, mHostTempBuffer.begin())),
                          thrust::make_zip_iterator(thrust::make_tuple(mMarketingTable.begin<date_tag>() + lEndIndex, mHostTempBuffer.end())));
}



}    // namespace abollo
