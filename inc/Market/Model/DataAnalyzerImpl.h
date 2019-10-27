#ifndef __ABOLLO_MARKET_MODEL_DATA_ANALYZER_IMPL_H__
#define __ABOLLO_MARKET_MODEL_DATA_ANALYZER_IMPL_H__



#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/transform.h>
#include <boost/circular_buffer.hpp>

#include "Market/Model/CircularMarketingTable.h"
#include "Market/Model/ColumnTraits.h"



namespace abollo
{



template <const uint8_t P, typename F, typename Tags>
class DataAnalyzerImpl;



template <const uint8_t P, typename F, typename... Tags>
class DataAnalyzerImpl<P, F, TableSchema<Tags...>> final
{
private:
    constexpr static std::size_t DEFAULT_BUFFER_COL_SIZE{1 << P};

    CircularMarketingTable<float, P, Tags...> mMarketingTable;

    mutable thrust::device_vector<F> mDeviceTempBuffer{DEFAULT_BUFFER_COL_SIZE};
    mutable thrust::host_vector<F> mHostTempBuffer{DEFAULT_BUFFER_COL_SIZE};

public:
    template <typename Iterator, typename Op>
    [[nodiscard]] const auto& Transform(Iterator aBeginIter, Iterator aEndIter, Op&& aSaxpyOp) const
    {
        const auto lSize = thrust::distance(aBeginIter, aEndIter);

        assert(lSize <= mDeviceTempBuffer.capacity());

        mDeviceTempBuffer.clear();

        thrust::transform(aBeginIter, aEndIter, mDeviceTempBuffer.begin(), aSaxpyOp);

        mHostTempBuffer.assign(mDeviceTempBuffer.begin(), mDeviceTempBuffer.begin() + lSize);

        return mHostTempBuffer;
    }

    [[nodiscard]] auto& Data() const
    {
        return mMarketingTable;
    }

    [[nodiscard]] auto& operator[](const std::size_t aIndex) const
    {
        return mHostTempBuffer[aIndex];
    }

    [[nodiscard]] std::size_t Size() const
    {
        return mMarketingTable.size();
    }

    template <typename U>
    void Append(U&& aData)
    {
        mMarketingTable.push_back(std::forward<U>(aData));
    }

    template <typename U>
    void Prepend(U&& aData)
    {
        mMarketingTable.push_front(std::forward<U>(aData));
    }
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_DATA_ANALYZER_IMPL_H__
