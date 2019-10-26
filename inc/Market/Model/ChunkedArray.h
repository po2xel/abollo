#ifndef __ABOLLO_MARKET_MODEL_CHUNKED_ARRAY_H__
#define __ABOLLO_MARKET_MODEL_CHUNKED_ARRAY_H__



#include <cstddef>
#include <cstdint>



namespace abollo
{



template <typename C, const std::size_t Size, typename Tag>
class ChunkedArray
{
public:
    using iterator       = typename C::iterator;
    using const_iterator = typename C::const_iterator;

private:
    C mColumn;

public:
    ChunkedArray() : mColumn(Size)
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

    auto& operator[](const std::size_t aIndex)
    {
        return mColumn[aIndex];
    }

    auto& operator[](const std::size_t aIndex) const
    {
        return mColumn[aIndex];
    }
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_CHUNKED_ARRAY_H__
