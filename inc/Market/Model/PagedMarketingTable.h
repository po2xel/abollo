#ifndef __ABOLLO_MARKET_MODEL_PAGED_MARKETING_TABLE_H__
#define __ABOLLO_MARKET_MODEL_PAGED_MARKETING_TABLE_H__



#include <date/date.h>
#include <thrust/host_vector.h>

#include "Market/Model/ColumnTraits.h"
#include "Market/Model/Table.h"



namespace abollo
{


template <typename T, const std::size_t P, typename Tag>
using HostColumn = Column<thrust::host_vector<T>, P, Tag>;



template <typename T, const std::size_t P, typename... Tags>
class PagedMarketingTable : private HostColumn<date::year_month_day, 1 << P, date_tag>, public Table<thrust::host_vector<T>, 1 << P, Tags...>
{
public:
    using Schema = TableSchema<date_tag, Tags...>;

private:
    using DateColumnType = HostColumn<date::year_month_day, 1 << P, date_tag>;

    constexpr static std::size_t PAGE_SIZE = 1 << P;

    std::size_t mSize{0};

    template <typename Tag, typename U>
    void push_back(U&& aValue)
    {
        using BaseType = std::conditional_t<std::is_same_v<date_tag, Tag>, DateColumnType, HostColumn<T, PAGE_SIZE, Tag>>;

        BaseType& lBaseColumn = *this;
        lBaseColumn[mSize]    = aValue.template Get<Tag>();
    }

    template <typename... Ts, typename U>
    void push_back(U&& aValue, TableSchema<Ts...>)
    {
        assert(mSize + 1 < PAGE_SIZE);

        (push_back<Ts>(std::forward<U>(aValue)), ...);

        ++mSize;
    }

public:
    template <typename U>
    void push_back(U&& aValue)
    {
        push_back(aValue, table_schema_v<date_tag, Tags...>);
    }

    template <typename Tag>
    [[nodiscard]] auto begin() const
    {
        using BaseType = std::conditional_t<std::is_same_v<date_tag, Tag>, DateColumnType, HostColumn<T, PAGE_SIZE, Tag>>;

        return BaseType::begin();
    }

    template <typename Tag>
    [[nodiscard]] auto end() const
    {
        using BaseType = std::conditional_t<std::is_same_v<date_tag, Tag>, DateColumnType, HostColumn<T, PAGE_SIZE, Tag>>;

        return BaseType::end();
    }

    void clear()
    {
        mSize = 0;
    }
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_PAGED_MARKETING_TABLE_H__
