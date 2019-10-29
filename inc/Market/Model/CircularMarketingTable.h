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



template <typename T, const uint8_t P, typename... Tags>
class CircularMarketingTable;



template <typename T, const uint8_t P, typename... Tags>
class CircularMarketingTable<T, P, TableSchema<Tags...>> : private Column<thrust::host_vector<date::year_month_day>, 1 << P, date_tag>,
                                                           public Table<thrust::device_vector<T>, 1 << P, remove_t<date_tag, Tags...>>
{
public:
    using Schema = TableSchema<Tags...>;

private:
    using DateColumn = Column<thrust::host_vector<date::year_month_day>, 1 << P, date_tag>;
    using BaseTable  = Table<thrust::device_vector<T>, 1 << P, remove_t<date_tag, Tags...>>;

    constexpr static std::size_t CAPACITY      = 1 << P;
    constexpr static std::size_t CAPACITY_MASK = CAPACITY - 1;

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

    template <typename Tag, typename Iterator>
    void Append(Iterator aBegin, Iterator aEnd)
    {
        const auto lDistance = std::distance(aBegin, aEnd);
        assert(lDistance >= 0 && static_cast<std::size_t>(lDistance) <= CAPACITY);

        using BaseType = std::conditional_t<std::is_same_v<date_tag, Tag>, DateColumn, DeviceColumn<T, CAPACITY, Tag>>;

        const auto lSize = std::min(CAPACITY - mLast, static_cast<std::size_t>(lDistance));
        thrust::copy_n(aBegin, lSize, BaseType::begin() + mLast);

        const auto lLeft = lDistance - lSize;

        if (lLeft > 0)
            thrust::copy_n(aBegin + lSize, lLeft, BaseType::begin());
    }

    template <typename Tag, typename Iterator>
    void Prepend(Iterator aBegin, Iterator aEnd)
    {
        const auto lDistance = std::distance(aBegin, aEnd);
        assert(lDistance >= 0 && static_cast<std::size_t>(lDistance) <= CAPACITY);

        using BaseType = std::conditional_t<std::is_same_v<date_tag, Tag>, DateColumn, DeviceColumn<T, CAPACITY, Tag>>;

        const auto lSize = std::min(mFirst, static_cast<std::size_t>(lDistance));
        thrust::copy_n(aEnd - lSize, lSize, BaseType::begin() + mFirst - lSize);

        const auto lLeft = lDistance - lSize;

        if (lLeft > 0)
            thrust::copy_n(aBegin, lLeft, BaseType::end() - lLeft);
    }

    template <typename U, typename V, typename... Ts>
    void push_back(U&& aValue, TableSchema<V, Ts...>)
    {
        Append<V>(aValue.template begin<V>(), aValue.template end<V>());

        if constexpr (sizeof...(Ts) > 0)
            push_back(std::forward<U>(aValue), table_schema_v<Ts...>);
    }

    template <typename U, typename V, typename... Ts>
    void push_front(U&& aValue, TableSchema<V, Ts...>)
    {
        Prepend<V>(aValue.template begin<V>(), aValue.template end<V>());

        if constexpr (sizeof...(Ts) > 0)
            push_front(std::forward<U>(aValue, table_schema_v<Ts...>));
    }

public:
    using BaseTable::begin;
    using BaseTable::end;

    template <typename Tag>
    [[nodiscard]] auto begin() const
    {
        using BaseType = std::conditional_t<std::is_same_v<date_tag, Tag>, DateColumn, DeviceColumn<T, CAPACITY, Tag>>;

        return BaseType::begin();
    }

    // [[nodiscard]] auto begin() const
    // {
    //     return thrust::make_zip_iterator(thrust::make_tuple(std::conditional_t<std::is_same_v<date_tag, Tags>, DateColumnType, DeviceColumn<T, CAPACITY, Tags>>::begin()...));
    // }

    template <typename Tag>
    [[nodiscard]] auto end() const
    {
        using BaseType = std::conditional_t<std::is_same_v<date_tag, Tag>, DateColumn, DeviceColumn<T, CAPACITY, Tag>>;

        return BaseType::end();
    }

    // [[nodiscard]] auto end() const
    // {
    //     return thrust::make_zip_iterator(thrust::make_tuple(std::conditional_t<std::is_same_v<date_tag, Tags>, DateColumnType, DeviceColumn<T, CAPACITY, Tags>>::end()...));
    // }

    template <typename U>
    void push_back(U&& aValue)
    {
        push_back(std::forward<U>(aValue), table_schema_v<date_tag, Tags...>);
        Forward(aValue.size());
    }

    template <typename U>
    void push_front(U&& aValue)
    {
        push_front(std::forward<U>(aValue), table_schema_v<date_tag, Tags...>);
        Backward(aValue.size());
    }

    [[nodiscard]] std::size_t size() const
    {
        return mSize;
    }

    template <typename Tag>
    [[nodiscard]] auto at(const std::size_t aIndex) const
    {
        using BaseType = std::conditional_t<std::is_same_v<date_tag, Tag>, DateColumn, DeviceColumn<T, CAPACITY, Tag>>;

        return BaseType::operator[](aIndex);
    }
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_CIRCULAR_MARKETING_TABLE_H__
