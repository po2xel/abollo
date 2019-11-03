#ifndef __ABOLLO_MARKET_MODEL_COLUMN_TRAITS_H__
#define __ABOLLO_MARKET_MODEL_COLUMN_TRAITS_H__



#include <tuple>
#include <type_traits>



namespace abollo
{



struct date_tag;
struct seq_tag;
struct open_tag;
struct close_tag;
struct low_tag;
struct high_tag;
struct pre_close_tag;
struct change_tag;
struct pct_change_tag;
struct volume_tag;
struct amount_tag;

struct price_tag;
struct log_price_tag;
struct log_volume_tag;


template <typename Tag>
struct ColumnTraits
{
    using tag = Tag;
};


template <typename Tag>
constexpr ColumnTraits<Tag> column_v{};



template <typename... Ts>
using tuple_cat_t = decltype(std::tuple_cat(std::declval<Ts>()...));


template <typename T, typename... Ts>
using remove_t = tuple_cat_t<std::conditional_t<std::is_same_v<T, Ts>, std::tuple<>, std::tuple<Ts>>...>;



template <typename... Tags>
struct TableSchema
{
};


template <typename... Tags>
struct TableSchema<std::tuple<Tags...>> : public TableSchema<Tags...>
{
};



template <typename... Tags>
constexpr TableSchema<Tags...> table_schema_v{};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_COLUMN_TRAITS_H__
