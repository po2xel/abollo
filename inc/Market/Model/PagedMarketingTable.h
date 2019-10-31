#ifndef __ABOLLO_MARKET_MODEL_PAGED_MARKETING_TABLE_H__
#define __ABOLLO_MARKET_MODEL_PAGED_MARKETING_TABLE_H__



#include <date/date.h>
#include <thrust/host_vector.h>

#include "Market/Model/ColumnTraits.h"
#include "Market/Model/Table.h"



namespace abollo
{


template <typename T, const uint32_t Cap, typename Tag>
using HostColumn = Column<thrust::host_vector<T>, Cap, Tag>;



template <typename T, const uint8_t P, typename... Tags>
class PagedMarketingTable;



template <typename T, const uint8_t P, typename... Tags>
class PagedMarketingTable<T, P, TableSchema<Tags...>> : private HostColumn<date::year_month_day, 1 << P, date_tag>,
                                                        public Table<thrust::host_vector<T>, 1 << P, remove_t<date_tag, Tags...>>
{
public:
    using Schema = TableSchema<Tags...>;

private:
    using DateColumn = HostColumn<date::year_month_day, 1 << P, date_tag>;
    using BaseTable  = Table<thrust::host_vector<T>, 1 << P, remove_t<date_tag, Tags...>>;

    constexpr static uint32_t CAPACITY = 1 << P;

    uint32_t mSize{0};

    template <typename Tag, typename U>
    auto push_back(U&& aValue)
    {
        using BaseType = std::conditional_t<std::is_same_v<date_tag, Tag>, DateColumn, HostColumn<T, CAPACITY, Tag>>;

        BaseType& lBaseColumn = *this;
        lBaseColumn[mSize]    = aValue.template Get<Tag>();

        return mSize;
    }

    template <typename U, typename... Ts>
    void push_back(U&& aValue, TableSchema<Ts...>)
    {
        assert(mSize + 1 < CAPACITY);

        // (push_back<Ts>(std::forward<U>(aValue)), ...);

        using expander = uint32_t[];
        (void)expander{(push_back<Ts>(std::forward<U>(aValue)))...};
    }

public:
    template <typename U>
    void push_back(U&& aValue)
    {
        push_back(std::forward<U>(aValue), table_schema_v<Tags...>);

        ++mSize;
    }

    template <typename Tag>
    [[nodiscard]] auto begin() const
    {
        using BaseType = std::conditional_t<std::is_same_v<date_tag, Tag>, DateColumn, HostColumn<T, CAPACITY, Tag>>;

        return BaseType::begin();
    }

    [[nodiscard]] auto begin() const
    {
        return thrust::make_zip_iterator(thrust::make_tuple(std::conditional_t<std::is_same_v<date_tag, Tags>, DateColumn, HostColumn<T, CAPACITY, Tags>>::begin()...));
    }

    template <typename Tag>
    [[nodiscard]] auto end() const
    {
        using BaseType = std::conditional_t<std::is_same_v<date_tag, Tag>, DateColumn, HostColumn<T, CAPACITY, Tag>>;

        return BaseType::end();
    }

    [[nodiscard]] auto end() const
    {
        return thrust::make_zip_iterator(thrust::make_tuple(std::conditional_t<std::is_same_v<date_tag, Tags>, DateColumn, HostColumn<T, CAPACITY, Tags>>::end()...));
    }

    [[nodiscard]] auto size() const
    {
        return mSize;
    }

    void clear()
    {
        mSize = 0;
    }
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_PAGED_MARKETING_TABLE_H__
