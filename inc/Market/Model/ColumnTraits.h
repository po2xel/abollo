#ifndef __ABOLLO_MARKET_MODEL_COLUMN_TRAITS_H__
#define __ABOLLO_MARKET_MODEL_COLUMN_TRAITS_H__



namespace abollo
{



struct date_tag;
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


template <typename Tag>
struct ColumnTraits
{
    using tag = Tag;
};


template <typename Tag>
constexpr ColumnTraits<Tag> column_v{};



template <typename... Tags>
struct TableSchema
{
};


template <typename... Tags>
constexpr  TableSchema<Tags...> table_schema_v{};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MODEL_COLUMN_TRAITS_H__
