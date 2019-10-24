#ifndef __ABOLLO_MARKET_MODEL_TABLE_H__
#define __ABOLLO_MARKET_MODEL_TABLE_H__



#include <algorithm>
#include <string>
#include <utility>

#include <date/date.h>
// #include <thrust/device_vector.h>
#include <thrust/host_vector.h>

#include "Market/Model/ChunkedArray.h"



namespace abollo
{



template <typename T, const std::size_t Cap, ColumnType... Ys>
class DeviceTable : private ChunkedArray<thrust::host_vector, date::year_month_day, Cap, ColumnType::eDate>, private ChunkedArray<thrust::host_vector, T, Cap, Ys>...
{
private:
    std::string mCode;

    std::size_t mFirst{0};
    std::size_t mLast{0};

public:
    explicit DeviceTable(std::string aCode) noexcept : mCode{std::move(aCode)}
    {
    }

    template <ColumnType Y>
    auto Begin()
    {
        return ChunkedArray<thrust::host_vector, T, Cap, Y>::begin();
    }

    template <ColumnType Y>
    auto End()
    {
        return ChunkedArray<thrust::host_vector, T, Cap, Y>::end();
    }

    template <ColumnType Y, typename Iterator>
    void PushBack(Iterator aBegin, Iterator aEnd)
    {
        const auto lDistance = std::distance(aBegin, aEnd);
        assert(lDistance >= 0 && static_cast<std::size_t>(lDistance) <= Cap);

        using BaseType = ChunkedArray<thrust::host_vector, T, Cap, Y>;

        const auto lSize = std::min(Cap - mLast, static_cast<std::size_t>(lDistance));

        thrust::copy_n(aBegin, lSize, BaseType::begin() + mLast);

        if (lSize < static_cast<std::size_t>(lDistance))
        {
            thrust::copy_n(aBegin + lSize, lDistance - lSize, BaseType::begin());
            mLast  = (mLast + lDistance) & (Cap - 1);
            mFirst = mLast;
        }
        else
        {
            mLast += lSize;
        }
    }
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_TABLE_H__
