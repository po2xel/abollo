#ifndef __ABOLLO_MARKET_MODEL_CHUNKED_ARRAY_H__
#define __ABOLLO_MARKET_MODEL_CHUNKED_ARRAY_H__



#include <cstddef>
#include <cstdint>



namespace abollo
{



template <typename C, const uint32_t Size, typename Tag>
class ChunkedArray
{
public:
    using iterator        = typename C::iterator;
    using const_iterator  = typename C::const_iterator;
    using reference       = typename C::reference;
    using const_reference = typename C::const_reference;

private:
    C mColumn;

public:
    ChunkedArray() : mColumn(Size)
    {
    }

    [[nodiscard]] auto begin() noexcept
    {
        return mColumn.begin();
    }

    [[nodiscard]] auto begin() const noexcept
    {
        return mColumn.begin();
    }

    [[nodiscard]] auto cbegin() const noexcept
    {
        return mColumn.cbegin();
    }

    [[nodiscard]] auto end() noexcept
    {
        return mColumn.end();
    }

    [[nodiscard]] auto end() const noexcept
    {
        return mColumn.end();
    }

    [[nodiscard]] auto cend() const noexcept
    {
        return mColumn.cend();
    }

    reference operator[](const uint32_t aIndex)
    {
        return mColumn[aIndex];
    }

    const_reference operator[](const uint32_t aIndex) const
    {
        return mColumn[aIndex];
    }
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_CHUNKED_ARRAY_H__
