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


void DataAnalyzer::LoadIndex(const std::string& aCode, const date::year_month_day& aStartDate, const date::year_month_day& aEndDate)
{
    mDataLoader.LoadIndex<RowType>(aCode, aStartDate, aEndDate, [this](const auto& aData) { mPagedTable.push_back(aData); });

    mImpl->Append(mPagedTable);
}


uint32_t DataAnalyzer::Size() const
{
    return mImpl->Size();
}


MarketDataFields DataAnalyzer::operator[](const uint32_t aIndex) const
{
    return (*mImpl)[aIndex];
}


template <>
std::pair<float, float> DataAnalyzer::MinMax<price_tag>(const uint32_t aStartIndex, const uint32_t aSize) const
{
    const auto& lData = mImpl->Data();

    assert(aStartIndex < lData.size());

    const auto lEndIndex = aStartIndex + std::min(aSize, Size() - aStartIndex);

    const auto lMinIter = thrust::min_element(lData.begin<low_tag>() + aStartIndex, lData.begin<low_tag>() + lEndIndex);
    const auto lMaxIter = thrust::max_element(lData.begin<high_tag>() + aStartIndex, lData.begin<high_tag>() + lEndIndex);

    return std::make_pair(*lMinIter, *lMaxIter);
}


template <>
std::pair<float, float> DataAnalyzer::MinMax<log_price_tag>(const uint32_t aStartIndex, const uint32_t aSize) const
{
    const auto lMinMax = MinMax<price_tag>(aStartIndex, aSize);

    return std::make_pair(std::logf(lMinMax.first), std::logf(lMinMax.second));
}


template <>
std::pair<float, float> DataAnalyzer::MinMax<volume_tag>(const uint32_t aStartIndex, const uint32_t aSize) const
{
    const auto& lData = mImpl->Data();

    assert(aStartIndex < lData.size());

    const auto lEndIndex = aStartIndex + std::min(aSize, Size() - aStartIndex);

    const auto lMinMaxIter = thrust::minmax_element(lData.begin<volume_tag>() + aStartIndex, lData.begin<volume_tag>() + lEndIndex);

    return std::make_pair(*lMinMaxIter.first, *lMinMaxIter.second);
}


template <>
std::pair<float, float> DataAnalyzer::MinMax<log_volume_tag>(const uint32_t aStartIndex, const uint32_t aSize) const
{
    const auto lMinMax = MinMax<volume_tag>(aStartIndex, aSize);

    return std::make_pair(std::logf(lMinMax.first), std::logf(lMinMax.second));
}


template <>
std::pair<DatePriceZipIterator, DatePriceZipIterator> DataAnalyzer::Saxpy<price_tag>(const uint32_t aStartIndex, const uint32_t aSize, const float aScaleX, const float aTransX,
                                                                                     const float aScaleY, const float aTransY, const float aScaleZ, const float aTransZ) const
{
    const auto& lData = mImpl->Data();

    assert(aStartIndex < lData.size());

    const auto lSize     = std::min(aSize, Size() - aStartIndex);
    const auto lEndIndex = aStartIndex + lSize;

    const auto lIter = lData.ibegin(aStartIndex);

    const auto& lResult = mImpl->Transform(lIter, lIter + lSize, [sx = aScaleX, tx = aTransX, sy = aScaleY, ty = aTransY, sz = aScaleZ, tz = aTransZ] __device__(auto&& a) {
        return MarketDataFields(
            {
                thrust::get<0>(a),    // index
                thrust::get<1>(a),    // open
                thrust::get<2>(a),    // close
                thrust::get<3>(a),    // low
                thrust::get<4>(a),    // high
                thrust::get<5>(a),    // volume
                thrust::get<6>(a)     // amount
            },
            {
                thrust::get<0>(a) * sx + tx,    // index
                thrust::get<1>(a) * sy + ty,    // open
                thrust::get<2>(a) * sy + ty,    // close
                thrust::get<3>(a) * sy + ty,    // low
                thrust::get<4>(a) * sy + ty,    // high
                thrust::get<5>(a) * sz + tz,    // volume
                thrust::get<6>(a) * sz + tz     // amount
            });
    });

    return std::make_pair(thrust::make_zip_iterator(thrust::make_tuple(lData.begin<date_tag>() + aStartIndex, lResult.begin())),
                          thrust::make_zip_iterator(thrust::make_tuple(lData.begin<date_tag>() + lEndIndex, lResult.end())));
}


template <>
std::pair<DatePriceZipIterator, DatePriceZipIterator> DataAnalyzer::Saxpy<log_price_tag>(const uint32_t aStartIndex, const uint32_t aSize, const float aScaleX, const float aTransX,
                                                                                         const float aScaleY, const float aTransY, const float aScaleZ, const float aTransZ) const
{
    const auto& lData = mImpl->Data();

    assert(aStartIndex < lData.size());

    const auto lSize     = std::min(aSize, Size() - aStartIndex);
    const auto lEndIndex = aStartIndex + lSize;

    const auto lIter = lData.ibegin(aStartIndex);

    const auto& lResult = mImpl->Transform(lIter, lIter + lSize, [sx = aScaleX, tx = aTransX, sy = aScaleY, ty = aTransY, sz = aScaleZ, tz = aTransZ] __device__(auto&& a) {
        return MarketDataFields(
            {
                thrust::get<0>(a),    // index
                thrust::get<1>(a),    // open
                thrust::get<2>(a),    // close
                thrust::get<3>(a),    // low
                thrust::get<4>(a),    // high
                thrust::get<5>(a),    // volume
                thrust::get<6>(a)     // amount
            },
            {
                thrust::get<0>(a) * sx + tx,          // index
                logf(thrust::get<1>(a)) * sy + ty,    // open
                logf(thrust::get<2>(a)) * sy + ty,    // close
                logf(thrust::get<3>(a)) * sy + ty,    // low
                logf(thrust::get<4>(a)) * sy + ty,    // high
                logf(thrust::get<5>(a)) * sz + tz,    // volume
                logf(thrust::get<6>(a)) * sz + tz     // amount
            });
    });

    return std::make_pair(thrust::make_zip_iterator(thrust::make_tuple(lData.begin<date_tag>() + aStartIndex, lResult.begin())),
                          thrust::make_zip_iterator(thrust::make_tuple(lData.begin<date_tag>() + lEndIndex, lResult.end())));
}



}    // namespace abollo