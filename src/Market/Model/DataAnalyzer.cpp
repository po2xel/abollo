#include "Market/Model/DataAnalyzer.h"

#include <soci/values.h>
#include <thrust/extrema.h>
#include <thrust/iterator/zip_iterator.h>
#include <thrust/tuple.h>

#include "Market/Model/DataAnalyzerImpl.h"



namespace
{



namespace detail
{



template <typename T, typename Tuple, uint32_t... Is>
constexpr auto tuple_cat_impl(T&& t, const Tuple& ts, std::index_sequence<Is...>)
{
    return thrust::make_tuple(std::forward<T>(t), thrust::get<Is>(ts)...);
}


template <typename T, typename... Ts>
constexpr auto tuple_cat(T&& t, const thrust::tuple<Ts...>& ts)
{
    constexpr auto tuple_size = thrust::tuple_size<thrust::tuple<Ts...>>::value;

    return tuple_cat_impl(std::forward<T>(t), ts, std::make_index_sequence<tuple_size>{});
}


template <typename T, typename... Ts>
constexpr auto tuple_cat(T t, Ts&&... ts)
{
    return thrust::make_tuple(t, std::forward<Ts>(ts)...);
}



}    // namespace detail



using RowType = abollo::Row<abollo::DataAnalyzer::DataSchema>;



}    // namespace



namespace soci
{



template <>
struct type_conversion<RowType>
{
    using base_type = values;

    static void from_base(const base_type& v, indicator /*ind*/, RowType& price)
    {
        price.Set<abollo::date_tag>(v.get<date::year_month_day>("date"));

        price.Set<abollo::seq_tag>(static_cast<float>(v.get<int>("seq")));
        price.Set<abollo::open_tag>(static_cast<float>(v.get<double>("open")));
        price.Set<abollo::close_tag>(static_cast<float>(v.get<double>("close")));
        price.Set<abollo::low_tag>(static_cast<float>(v.get<double>("low")));
        price.Set<abollo::high_tag>(static_cast<float>(v.get<double>("high")));
        price.Set<abollo::volume_tag>(static_cast<float>(v.get<double>("volume") / 1000000.f));
        price.Set<abollo::amount_tag>(static_cast<float>(v.get<double>("amount") / 1000000.f));
    }
};


template <>
struct type_conversion<date::year_month_day>
{
    using base_type = std::tm;

    static void from_base(const base_type& in, const indicator ind, date::year_month_day& out)
    {
        if (ind == i_null)
            throw soci_error("Null value not allowed for this type");

        out = date::year{in.tm_year + 1900} / (in.tm_mon + 1) / in.tm_mday;
    }

    static void to_base(const date::year_month_day& in, base_type& out, indicator& ind)
    {
        out.tm_mday = static_cast<int>(static_cast<unsigned>(in.day()));
        out.tm_mon  = static_cast<int>(static_cast<unsigned>(in.month()) - 1);
        out.tm_year = static_cast<int>(static_cast<int>(in.year()) - 1900);

        ind = i_ok;
    }
};


// template <>
// struct type_conversion<std::string_view>
// {
//     using base_type = std::string;
//
//     static void from_base(const base_type& in, const indicator ind, std::string_view& out)
//     {
//         if (ind == i_null)
//             throw soci_error("Null value not allowed for this type");
//
//         out = in;
//     }
//
//     static void to_base(const std::string_view& in, base_type& out, indicator& ind)
//     {
//         out = in;
//
//         ind = i_ok;
//     }
// };



}    // namespace soci



namespace abollo
{



DataAnalyzer::DataAnalyzer() : mImpl{std::make_unique<ImplType>()}
{
}


DataAnalyzer::~DataAnalyzer() = default;


std::pair<std::uint32_t, std::uint32_t> DataAnalyzer::LoadIndex(const std::string& aCode, const uint32_t& aOffset, const uint32_t& aLimit)
{
    PagedTableType lPagedTable;

    mDataLoader.LoadIndex<RowType>(aCode, aOffset, aLimit, [&lPagedTable](const auto& aData) { lPagedTable.push_back(aData); });

    mStartSeq = static_cast<uint32_t>(lPagedTable.back<seq_tag>());
    mEndSeq   = static_cast<uint32_t>(lPagedTable.front<seq_tag>());

    mImpl->Append(lPagedTable);

    return {mStartSeq, mEndSeq};
}


uint32_t DataAnalyzer::Size() const
{
    return mImpl->Size();
}


template <>
std::pair<float, float> DataAnalyzer::Range<seq_tag>() const
{
    return mImpl->Range<seq_tag>();
}


MarketDataFields DataAnalyzer::operator[](const uint32_t aIndex) const
{
    return (*mImpl)[aIndex];
}


template <>
std::pair<float, float> DataAnalyzer::MinMax<price_tag>(const uint32_t aStartIndex, const uint32_t aEndIndex) const
{
    const auto lRange = Normalize(aStartIndex, aEndIndex);

    const auto& lData   = mImpl->Data();
    const auto lMinIter = thrust::min_element(lData.begin<low_tag>() + lRange.first, lData.begin<low_tag>() + lRange.second);
    const auto lMaxIter = thrust::max_element(lData.begin<high_tag>() + lRange.first, lData.begin<high_tag>() + lRange.second);

    return std::make_pair(*lMinIter, *lMaxIter);
}


template <>
std::pair<float, float> DataAnalyzer::MinMax<log_price_tag>(const uint32_t aStartIndex, const uint32_t aEndIndex) const
{
    const auto lMinMax = MinMax<price_tag>(aStartIndex, aEndIndex);

    return std::make_pair(std::logf(lMinMax.first), std::logf(lMinMax.second));
}


template <>
std::pair<float, float> DataAnalyzer::MinMax<volume_tag>(const uint32_t aStartIndex, const uint32_t aEndIndex) const
{
    const auto lRange = Normalize(aStartIndex, aEndIndex);

    const auto& lData      = mImpl->Data();
    const auto lMinMaxIter = thrust::minmax_element(lData.begin<volume_tag>() + lRange.first, lData.begin<volume_tag>() + lRange.second);

    return std::make_pair(*lMinMaxIter.first, *lMinMaxIter.second);
}


template <>
std::pair<float, float> DataAnalyzer::MinMax<log_volume_tag>(const uint32_t aStartIndex, const uint32_t aEndIndex) const
{
    const auto lMinMax = MinMax<volume_tag>(aStartIndex, aEndIndex);

    return std::make_pair(std::logf(lMinMax.first), std::logf(lMinMax.second));
}


template <>
DatePricePair DataAnalyzer::Saxpy<price_tag>(const uint32_t aStartIndex, const uint32_t aEndIndex, const float aScaleX, const float aTransX, const float aScaleY,
                                             const float aTransY, const float aScaleZ, const float aTransZ) const
{
    const auto lRange = Normalize(aStartIndex, aEndIndex);

    const auto& lData = mImpl->Data();
    const auto lIter  = lData.begin();

    const auto& lResult =
        mImpl->Transform(lIter + lRange.first, lIter + lRange.second, [sx = aScaleX, tx = aTransX, sy = aScaleY, ty = aTransY, sz = aScaleZ, tz = aTransZ] __device__(auto&& a) {
            return MarketDataFields(
                {
                    thrust::get<0>(a),    // seq
                    thrust::get<1>(a),    // open
                    thrust::get<2>(a),    // close
                    thrust::get<3>(a),    // low
                    thrust::get<4>(a),    // high
                    thrust::get<5>(a),    // volume
                    thrust::get<6>(a)     // amount
                },
                {
                    thrust::get<0>(a) * sx + tx,    // seq
                    thrust::get<1>(a) * sy + ty,    // open
                    thrust::get<2>(a) * sy + ty,    // close
                    thrust::get<3>(a) * sy + ty,    // low
                    thrust::get<4>(a) * sy + ty,    // high
                    thrust::get<5>(a) * sz + tz,    // volume
                    thrust::get<6>(a) * sz + tz     // amount
                });
        });

    return std::make_pair(thrust::make_zip_iterator(thrust::make_tuple(lData.begin<date_tag>() + lRange.first, lResult.begin())),
        thrust::make_zip_iterator(thrust::make_tuple(lData.begin<date_tag>() + lRange.second, lResult.end())));
}


template <>
DatePricePair DataAnalyzer::Saxpy<log_price_tag>(const uint32_t aStartIndex, const uint32_t aEndIndex, const float aScaleX, const float aTransX, const float aScaleY,
                                                 const float aTransY, const float aScaleZ, const float aTransZ) const
{
    const auto lRange = Normalize(aStartIndex, aEndIndex);

    const auto& lData = mImpl->Data();
    const auto lIter  = lData.begin();

    const auto& lResult =
        mImpl->Transform(lIter + lRange.first, lIter + lRange.second, [sx = aScaleX, tx = aTransX, sy = aScaleY, ty = aTransY, sz = aScaleZ, tz = aTransZ] __device__(auto&& a) {
            return MarketDataFields(
                {
                    thrust::get<0>(a),    // seq
                    thrust::get<1>(a),    // open
                    thrust::get<2>(a),    // close
                    thrust::get<3>(a),    // low
                    thrust::get<4>(a),    // high
                    thrust::get<5>(a),    // volume
                    thrust::get<6>(a)     // amount
                },
                {
                    thrust::get<0>(a) * sx + tx,          // seq
                    logf(thrust::get<1>(a)) * sy + ty,    // open
                    logf(thrust::get<2>(a)) * sy + ty,    // close
                    logf(thrust::get<3>(a)) * sy + ty,    // low
                    logf(thrust::get<4>(a)) * sy + ty,    // high
                    logf(thrust::get<5>(a)) * sz + tz,    // volume
                    logf(thrust::get<6>(a)) * sz + tz     // amount
                });
        });

    return std::make_pair(thrust::make_zip_iterator(thrust::make_tuple(lData.begin<date_tag>() + lRange.first, lResult.begin())),
                          thrust::make_zip_iterator(thrust::make_tuple(lData.begin<date_tag>() + lRange.second, lResult.end())));
}



}    // namespace abollo