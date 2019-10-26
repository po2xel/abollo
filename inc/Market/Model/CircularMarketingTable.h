#ifndef __ABOLLO_MARKET_MODEL_CIRCULAR_MARKETING_TABLE_H__
#define __ABOLLO_MARKET_MODEL_CIRCULAR_MARKETING_TABLE_H__



#include <thrust/device_vector.h>
#include <thrust/host_vector.h>

#include "Market/Model/ColumnTraits.h"
#include "Market/Model/Table.h"



namespace abollo
{



template <typename T, const std::size_t Cap, typename Tag>
using DeviceColumn = Column<thrust::device_vector<T>, Cap, Tag>;



template <typename T, const std::size_t P, typename... Tags>
class CircularMarketingTable : private Column<thrust::host_vector<date::year_month_day>, 1 << P, date_tag>, public Table<thrust::device_vector<T>, 1 << P, Tags...>
{
public:
    using Schema = TableSchema<date_tag, Tags...>;

private:
    using DateColumnType = Column<thrust::host_vector<date::year_month_day>, 1 << P, date_tag>;

    constexpr static std::size_t CAPACITY      = 1 << P;
    constexpr static std::size_t CAPACITY_MASK = CAPACITY - 1;

    std::string mCode;

    std::size_t mFirst{0};
    std::size_t mLast{0};
    std::size_t mSize{0};

    [[nodiscard]] auto SpaceLeft() const
    {
        return mFirst <= mLast ? CAPACITY + mFirst - mLast : mFirst - mLast;
    }

    [[nodiscard]] auto Full() const
    {
        return CAPACITY == mSize;
    }

    void Forward(const std::size_t aSize)
    {
        if (mLast + aSize >= CAPACITY_MASK)
            mLast = (mLast + aSize) & CAPACITY_MASK;
        else
            mLast += aSize;

        mSize = std::min(mSize + aSize, CAPACITY);

        if (Full())
            mFirst = mLast;
    }

    void Backward(const std::size_t aSize)
    {
        if (mFirst >= aSize)
            mFirst -= aSize;
        else
            mFirst = (mFirst + CAPACITY - aSize) & CAPACITY_MASK;

        mSize = std::min(mSize + aSize, CAPACITY);

        if (Full())
            mLast = mFirst;
    }

public:
    explicit CircularMarketingTable(std::string aCode) noexcept : mCode{std::move(aCode)}
    {
    }

    template <typename Tag>
    auto begin() const
    {
        using BaseType = std::conditional_t<std::is_same_v<date_tag, Tag>, DateColumnType, DeviceColumn<T, CAPACITY, Tag>>;

        return BaseType::begin();
    }

    template <typename Tag>
    auto end() const
    {
        using BaseType = std::conditional_t<std::is_same_v<date_tag, Tag>, DateColumnType, DeviceColumn<T, CAPACITY, Tag>>;

        return BaseType::end();
    }

    template <typename Tag, typename Iterator>
    void Append(Iterator aBegin, Iterator aEnd)
    {
        const auto lDistance = std::distance(aBegin, aEnd);
        assert(lDistance >= 0 && static_cast<std::size_t>(lDistance) <= CAPACITY);

        using BaseType = std::conditional_t<std::is_same_v<date_tag, Tag>, DateColumnType, DeviceColumn<T, CAPACITY, Tag>>;

        const auto lSize = std::min(CAPACITY - mLast, static_cast<std::size_t>(lDistance));
        thrust::copy_n(aBegin, lSize, BaseType::begin() + mLast);

        const auto lLeft = lDistance - lSize;

        if (lLeft > 0)
            thrust::copy_n(aBegin + lSize, lLeft, BaseType::begin());

        Forward(lDistance);
    }

    template <typename Tag, typename Iterator>
    void Prepend(Iterator aBegin, Iterator aEnd)
    {
        const auto lDistance = std::distance(aBegin, aEnd);
        assert(lDistance >= 0 && static_cast<std::size_t>(lDistance) <= CAPACITY);

        using BaseType = std::conditional_t<std::is_same_v<date_tag, Tag>, DateColumnType, DeviceColumn<T, CAPACITY, Tag>>;

        const auto lSize = std::min(mFirst, static_cast<std::size_t>(lDistance));
        thrust::copy_n(aEnd - lSize, lSize, BaseType::begin() + mFirst - lSize);

        const auto lLeft = lDistance - lSize;

        if (lLeft > 0)
            thrust::copy_n(aBegin, lLeft, BaseType::end() - lLeft);

        Backward(lDistance);
    }
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_CIRCULAR_MARKETING_TABLE_H__
