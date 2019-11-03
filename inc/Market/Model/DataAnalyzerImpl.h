#ifndef __ABOLLO_MARKET_MODEL_DATA_ANALYZER_IMPL_H__
#define __ABOLLO_MARKET_MODEL_DATA_ANALYZER_IMPL_H__



#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/transform.h>
#include <boost/circular_buffer.hpp>

#include "Market/Model/CircularMarketingTable.h"



namespace abollo
{



template <const uint8_t P, typename T, typename S>
class DataAnalyzerImpl final
{
private:
    constexpr static uint32_t DEFAULT_BUFFER_COL_SIZE{1 << P};

    CircularMarketingTable<float, P, S> mMarketingTable;

    mutable thrust::device_vector<T> mDeviceTempBuffer{DEFAULT_BUFFER_COL_SIZE};
    mutable thrust::host_vector<T> mHostTempBuffer{DEFAULT_BUFFER_COL_SIZE};

public:
    template <typename Iterator, typename Op>
    [[nodiscard]] const auto& Transform(Iterator aBeginIter, Iterator aEndIter, Op&& aSaxpyOp) const
    {
        const auto lSize = thrust::distance(aBeginIter, aEndIter);

        assert(static_cast<uint32_t>(lSize) <= mDeviceTempBuffer.capacity());

        mDeviceTempBuffer.clear();

        thrust::transform(aBeginIter, aEndIter, mDeviceTempBuffer.begin(), aSaxpyOp);

        mHostTempBuffer.assign(mDeviceTempBuffer.begin(), mDeviceTempBuffer.begin() + lSize);

        return mHostTempBuffer;
    }

    [[nodiscard]] auto& Data() const
    {
        return mMarketingTable;
    }

    [[nodiscard]] auto& operator[](const uint32_t aIndex) const
    {
        assert(aIndex < mHostTempBuffer.size());

        return mHostTempBuffer[aIndex];
    }

    [[nodiscard]] auto Size() const
    {
        return mMarketingTable.size();
    }

    template <typename U>
    void Append(U&& aData)
    {
        mMarketingTable.push_back(std::forward<U>(aData));
    }

    template <typename T>
    std::pair<float, float> Range() const
    {
        return {mMarketingTable.template front<T>(), mMarketingTable.template back<T>()};
    }

    template <typename U>
    void Prepend(U&& aData)
    {
        mMarketingTable.push_front(std::forward<U>(aData));
    }
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_DATA_ANALYZER_IMPL_H__
