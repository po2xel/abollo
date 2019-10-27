#ifndef __ABOLLO_MARKET_MODEL_DATA_ANALYZER_IMPL_INL_H__
#define __ABOLLO_MARKET_MODEL_DATA_ANALYZER_IMPL_INL_H__



#include <thrust/extrema.h>
#include <thrust/functional.h>
#include <thrust/host_vector.h>
#include <thrust/iterator/zip_iterator.h>
#include <thrust/transform.h>
#include <thrust/tuple.h>

#include "Market/Model/TradeDate.h"



namespace abollo
{



template <typename... Tags>
std::tuple<float, float, float, float> DataAnalyzerImpl<TableSchema<Tags...>>::MinMax(const std::size_t aStartIndex, const std::size_t aSize) const
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


template <typename... Tags>
std::pair<float, float> DataAnalyzerImpl<TableSchema<Tags...>>::MinMax(const std::size_t aStartIndex, const std::size_t aSize, const ColumnTraits<price_tag>) const
{
    if (aStartIndex > mPriceCount)
        throw std::out_of_range("Start index out of range.");

    const auto lEndIndex = aStartIndex + std::min(aSize, mPriceCount - aStartIndex);

    const auto lMinIter = thrust::min_element(mMarketingTable.template begin<low_tag>() + aStartIndex, mMarketingTable.template begin<low_tag>() + lEndIndex);
    const auto lMaxIter = thrust::min_element(mMarketingTable.template begin<high_tag>() + aStartIndex, mMarketingTable.template begin<high_tag>() + lEndIndex);

    return std::make_pair(*lMinIter, *lMaxIter);
}

template <typename... Tags>
std::pair<DatePriceZipIterator, DatePriceZipIterator> DataAnalyzerImpl<TableSchema<Tags...>>::Saxpy(const std::size_t aStartIndex, const std::size_t aSize, const float aScaleX,
                                                                                                    const float aTransX, const float aScaleY, const float aTransY,
                                                                                                    const float aScaleZ, const float aTransZ) const
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

    return std::make_pair(thrust::make_zip_iterator(thrust::make_tuple(mMarketingTable.template begin<date_tag>() + aStartIndex, mHostTempBuffer.begin())),
                          thrust::make_zip_iterator(thrust::make_tuple(mMarketingTable.template begin<date_tag>() + lEndIndex, mHostTempBuffer.end())));
}



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_DATA_ANALYZER_IMPL_INL_H__