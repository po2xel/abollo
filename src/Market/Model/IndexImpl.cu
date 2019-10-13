#include "Market/Model/IndexImpl.h"

#include "Market/Model/TradeDate.h"


#include <thrust/device_vector.h>
#include <thrust/extrema.h>
#include <thrust/functional.h>
#include <thrust/host_vector.h>
#include <thrust/iterator/zip_iterator.h>
#include <thrust/transform.h>
#include <thrust/tuple.h>


namespace abollo
{



void IndexImpl::LoadMinMax(const date::year_month_day& aStartDate, const date::year_month_day& aEndDate)
{
    using soci::into;
    using soci::use;

    mMinMaxStmt.exchange(use(aStartDate, "start"));
    mMinMaxStmt.exchange(use(aEndDate, "end"));

    double lMinPrice{0}, lMaxPrice{0}, lMinVolume{0}, lMaxVolume{0}, lMinAmount{0}, lMaxAmount{0};
    mMinMaxStmt.exchange(into(lMinPrice));
    mMinMaxStmt.exchange(into(lMaxPrice));
    mMinMaxStmt.exchange(into(lMinVolume));
    mMinMaxStmt.exchange(into(lMaxVolume));
    mMinMaxStmt.exchange(into(lMinAmount));
    mMinMaxStmt.exchange(into(lMaxAmount));

    mMinMaxStmt.define_and_bind();
    mMinMaxStmt.execute(true);
    mMinMaxStmt.bind_clean_up();

    mMinMax = std::make_tuple(static_cast<float>(lMinPrice), static_cast<float>(lMaxPrice), static_cast<float>(lMinVolume / 1000000.f), static_cast<float>(lMaxVolume / 1000000.f),
                              static_cast<float>(lMinAmount / 1000000.f), static_cast<float>(lMaxAmount / 1000000.f));
}


void IndexImpl::LoadIndex(const date::year_month_day& aStartDate, const date::year_month_day& aEndDate)
{
    using soci::into;
    using soci::use;

    Price lPrice{};

    mIndexDailyStmt.exchange(use(aStartDate, "start"));
    mIndexDailyStmt.exchange(use(aEndDate, "end"));
    mIndexDailyStmt.exchange(into(lPrice));

    mIndexDailyStmt.define_and_bind();
    mIndexDailyStmt.execute();

    thrust::host_vector<float> lPriceData{DEFAULT_BUFFER_CAPACITY};

    while (mIndexDailyStmt.fetch())
    {
        mDateBuffer.push_back(lPrice.date);

        lPriceData[mPriceCount]                               = lPrice.open;
        lPriceData[mPriceCount + DEFAULT_BUFFER_COL_SIZE]     = lPrice.close;
        lPriceData[mPriceCount + DEFAULT_BUFFER_COL_SIZE * 2] = lPrice.low;
        lPriceData[mPriceCount + DEFAULT_BUFFER_COL_SIZE * 3] = lPrice.high;
        lPriceData[mPriceCount + DEFAULT_BUFFER_COL_SIZE * 4] = lPrice.volume;
        lPriceData[mPriceCount + DEFAULT_BUFFER_COL_SIZE * 5] = lPrice.amount;

        ++mPriceCount;
    }

    mIndexDailyStmt.bind_clean_up();

    mDeviceBuffer = lPriceData;
}


std::tuple<float, float, float, float> IndexImpl::MinMax(const std::size_t aStartIndex, const std::size_t aSize) const
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


std::tuple<float, float, float, float> IndexImpl::MinMax(const date::year_month_day& /*aStartDate*/, const date::year_month_day& /*aEndDate*/) const
{
    return std::make_tuple(1.f, .1f, 1.f, 1.f);
}


std::pair<DatePriceZipIterator, DatePriceZipIterator> IndexImpl::Saxpy(const std::size_t aStartIndex, const std::size_t aSize, const float aScaleX, const float aTransX,
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

    return std::make_pair(thrust::make_zip_iterator(thrust::make_tuple(mDateBuffer.begin() + aStartIndex, mHostTempBuffer.begin())),
                          thrust::make_zip_iterator(thrust::make_tuple(mDateBuffer.begin() + lEndIndex, mHostTempBuffer.end())));
}



}    // namespace abollo
