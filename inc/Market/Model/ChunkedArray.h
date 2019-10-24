#ifndef __ABOLLO_MARKET_MODEL_CHUNKED_ARRAY_H__
#define __ABOLLO_MARKET_MODEL_CHUNKED_ARRAY_H__



#include <cstddef>
#include <cstdint>



namespace abollo
{



enum class ColumnType : uint8_t
{
    eDate,
    eOpen,
    eClose,
    eLow,
    eHigh,
    ePreClose,
    eChange,
    ePctChange,
    eVolume,
    eAmount
};



template <template <typename> class C, typename T, const std::size_t Cap, ColumnType Y>
class ChunkedArray
{
private:
    C<T> mColumn;

public:
    ChunkedArray() : mColumn(Cap)
    {
    }

    auto begin() noexcept
    {
        return mColumn.begin();
    }

    auto begin() const noexcept
    {
        return mColumn.begin();
    }

    auto cbegin() const noexcept
    {
        return mColumn.cbegin();
    }

    auto end() noexcept
    {
        return mColumn.end();
    }

    auto end() const noexcept
    {
        return mColumn.end();
    }

    auto cend() const noexcept
    {
        return mColumn.cend();
    }
};



}    // namespace abollo


#endif    // __ABOLLO_MARKET_MODEL_CHUNKED_ARRAY_H__
