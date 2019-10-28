#include "Market/Model/DataAnalyzer.h"

#include <cmath>

#include <soci/values.h>
#include <thrust/extrema.h>
#include <thrust/iterator/zip_iterator.h>
#include <thrust/tuple.h>

#include "Market/Model/DataAnalyzerImpl.h"
#include "Market/Model/PagedMarketingTable.h"



namespace
{



using RowType = abollo::Row<abollo::DataAnalyzer::DataSchema>;



}



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


std::pair<DatePriceZipIterator, DatePriceZipIterator> DataAnalyzer::Saxpy(const std::size_t aStartIndex, const std::size_t aSize, const float aScaleX, const float aTransX,
                                                                          const float aScaleY, const float aTransY, const float aScaleZ, const float aTransZ) const
{
    const auto& lData = mImpl->Data();

    assert(aStartIndex < lData.size());

    const auto lSize     = std::min(aSize, Size() - aStartIndex);
    const auto lEndIndex = aStartIndex + lSize;

    const thrust::counting_iterator<int> lCounterBegin{static_cast<int>(aStartIndex)};
    const thrust::counting_iterator<int> lCounterEnd{static_cast<int>(aStartIndex + lSize)};

    const auto lBeginIter = thrust::make_zip_iterator(make_tuple(lCounterBegin,                              // index
                                                                 lData.begin<open_tag>() + aStartIndex,      // open
                                                                 lData.begin<close_tag>() + aStartIndex,     // close
                                                                 lData.begin<low_tag>() + aStartIndex,       // low
                                                                 lData.begin<high_tag>() + aStartIndex,      // high
                                                                 lData.begin<volume_tag>() + aStartIndex,    // volume
                                                                 lData.begin<amount_tag>() + aStartIndex     // amount
                                                                 ));

    const auto lEndIter = thrust::make_zip_iterator(make_tuple(lCounterEnd,                              // index
                                                               lData.begin<open_tag>() + lEndIndex,      // open
                                                               lData.begin<close_tag>() + lEndIndex,     // close
                                                               lData.begin<low_tag>() + lEndIndex,       // low
                                                               lData.begin<high_tag>() + lEndIndex,      // high
                                                               lData.begin<volume_tag>() + lEndIndex,    // volume
                                                               lData.begin<amount_tag>() + lEndIndex     // amount
                                                               ));

    const auto& lResult = mImpl->Transform(lBeginIter, lEndIter, [sx = aScaleX, tx = aTransX, sy = aScaleY, ty = aTransY, sz = aScaleZ, tz = aTransZ] __device__(auto&& a) {
        return MarketDataFields(thrust::get<0>(a) * sx + tx,     // index
                                thrust::get<1>(a) * sy + ty,     // open
                                thrust::get<2>(a) * sy + ty,     // close
                                thrust::get<3>(a) * sy + ty,     // low
                                thrust::get<4>(a) * sy + ty,     // high
                                thrust::get<5>(a) * sz + tz,     // volume
                                thrust::get<6>(a) * sz + tz);    // amount
    });

    return std::make_pair(thrust::make_zip_iterator(thrust::make_tuple(lData.begin<date_tag>() + aStartIndex, lResult.begin())),
                          thrust::make_zip_iterator(thrust::make_tuple(lData.begin<date_tag>() + lEndIndex, lResult.end())));
}


std::pair<DatePriceZipIterator, DatePriceZipIterator> DataAnalyzer::LogSaxpy(const std::size_t aStartIndex, const std::size_t aSize, const float aScaleX, const float aTransX,
                                                                             const float aScaleY, const float aTransY, const float aScaleZ, const float aTransZ) const
{
    const auto& lData = mImpl->Data();

    auto t = lData.begin();
    for (auto i = 0; i < 10; ++i, ++t)
    {
        const auto lt = *t;
        std::cout << lt.get<0>() << "\t" << lt.get<1>() << "\t" << lt.get<2>()  << "\t" << lt.get<3>() << "\t" << lt.get<4>() << "\t" << lt.get<5>() << "\t" << lt.get<6>() << std::endl;
    }

    t = lData.end();

    assert(aStartIndex < lData.size());

    const auto lSize     = std::min(aSize, Size() - aStartIndex);
    const auto lEndIndex = aStartIndex + lSize;

    const thrust::counting_iterator<int> lCounterBegin{static_cast<int>(aStartIndex)};
    const thrust::counting_iterator<int> lCounterEnd{static_cast<int>(aStartIndex + lSize)};

    const auto lBeginIter = thrust::make_zip_iterator(make_tuple(lCounterBegin,                              // index
                                                                 lData.begin<open_tag>() + aStartIndex,      // open
                                                                 lData.begin<close_tag>() + aStartIndex,     // close
                                                                 lData.begin<low_tag>() + aStartIndex,       // low
                                                                 lData.begin<high_tag>() + aStartIndex,      // high
                                                                 lData.begin<volume_tag>() + aStartIndex,    // volume
                                                                 lData.begin<amount_tag>() + aStartIndex     // amount
                                                                 ));

    const auto lEndIter = thrust::make_zip_iterator(make_tuple(lCounterEnd,                              // index
                                                               lData.begin<open_tag>() + lEndIndex,      // open
                                                               lData.begin<close_tag>() + lEndIndex,     // close
                                                               lData.begin<low_tag>() + lEndIndex,       // low
                                                               lData.begin<high_tag>() + lEndIndex,      // high
                                                               lData.begin<volume_tag>() + lEndIndex,    // volume
                                                               lData.begin<amount_tag>() + lEndIndex     // amount
                                                               ));

    const auto& lResult = mImpl->Transform(lBeginIter, lEndIter, [sx = aScaleX, tx = aTransX, sy = aScaleY, ty = aTransY, sz = aScaleZ, tz = aTransZ] __device__(auto&& a) {
        return MarketDataFields(thrust::get<0>(a) * sx + tx,           // index
                                logf(thrust::get<1>(a)) * sy + ty,     // open
                                logf(thrust::get<2>(a)) * sy + ty,     // close
                                logf(thrust::get<3>(a)) * sy + ty,     // low
                                logf(thrust::get<4>(a)) * sy + ty,     // high
                                logf(thrust::get<5>(a)) * sz + tz,     // volume
                                logf(thrust::get<6>(a)) * sz + tz);    // amount
    });

    return std::make_pair(thrust::make_zip_iterator(thrust::make_tuple(lData.begin<date_tag>() + aStartIndex, lResult.begin())),
                          thrust::make_zip_iterator(thrust::make_tuple(lData.begin<date_tag>() + lEndIndex, lResult.end())));
}


MarketDataFields DataAnalyzer::operator[](const std::size_t aIndex) const
{
    const auto& lData = mImpl->Data();

    assert(aIndex < lData.size());

    if (mMarketingDataCache.Contains(aIndex))
        return mMarketingDataCache[aIndex];

    const auto aStartIndex = 0u;
    const auto lSize       = 512u;
    const auto lEndIndex   = aStartIndex + lSize;

    const thrust::counting_iterator<int> lCounterBegin{static_cast<int>(aStartIndex)};
    const thrust::counting_iterator<int> lCounterEnd{static_cast<int>(aStartIndex + lSize)};

    const auto lBeginIter = thrust::make_zip_iterator(make_tuple(lCounterBegin,                              // index
                                                                 lData.begin<open_tag>() + aStartIndex,      // open
                                                                 lData.begin<close_tag>() + aStartIndex,     // close
                                                                 lData.begin<low_tag>() + aStartIndex,       // low
                                                                 lData.begin<high_tag>() + aStartIndex,      // high
                                                                 lData.begin<volume_tag>() + aStartIndex,    // volume
                                                                 lData.begin<amount_tag>() + aStartIndex     // amount
                                                                 ));

    const auto lEndIter = thrust::make_zip_iterator(make_tuple(lCounterEnd,                              // index
                                                               lData.begin<open_tag>() + lEndIndex,      // open
                                                               lData.begin<close_tag>() + lEndIndex,     // close
                                                               lData.begin<low_tag>() + lEndIndex,       // low
                                                               lData.begin<high_tag>() + lEndIndex,      // high
                                                               lData.begin<volume_tag>() + lEndIndex,    // volume
                                                               lData.begin<amount_tag>() + lEndIndex     // amount
                                                               ));

    mMarketingDataCache.Assign(lBeginIter, lEndIter, aStartIndex);

    return mMarketingDataCache[aIndex];
}


MarketDataFields DataAnalyzer::Get(const std::size_t aIndex) const
{
    return (*mImpl)[aIndex];
}


std::size_t DataAnalyzer::Size() const
{
    return mImpl->Size();
}


std::pair<float, float> DataAnalyzer::MinMax(const std::size_t aStartIndex, const std::size_t aSize, ColumnTraits<price_tag>) const
{
    const auto& lData = mImpl->Data();

    assert(aStartIndex < lData.size());

    const auto lEndIndex = aStartIndex + std::min(aSize, Size() - aStartIndex);

    const auto lMinIter = thrust::min_element(lData.begin<low_tag>() + aStartIndex, lData.begin<low_tag>() + lEndIndex);
    const auto lMaxIter = thrust::max_element(lData.begin<high_tag>() + aStartIndex, lData.begin<high_tag>() + lEndIndex);

    return std::make_pair(*lMinIter, *lMaxIter);
}


std::pair<float, float> DataAnalyzer::MinMax(const std::size_t aStartIndex, const std::size_t aSize, ColumnTraits<log_price_tag>) const
{
    const auto lMinMax = MinMax(aStartIndex, aSize, column_v<price_tag>);

    return std::make_pair(std::logf(lMinMax.first), std::logf(lMinMax.second));
}


std::pair<float, float> DataAnalyzer::MinMax(const std::size_t aStartIndex, const std::size_t aSize, ColumnTraits<volume_tag>) const
{
    const auto& lData = mImpl->Data();

    assert(aStartIndex < lData.size());

    const auto lEndIndex = aStartIndex + std::min(aSize, Size() - aStartIndex);

    const auto lMinMaxIter = thrust::minmax_element(lData.begin<volume_tag>() + aStartIndex, lData.begin<volume_tag>() + lEndIndex);

    return std::make_pair(*lMinMaxIter.first, *lMinMaxIter.second);
}


std::pair<float, float> DataAnalyzer::MinMax(const std::size_t aStartIndex, const std::size_t aSize, ColumnTraits<log_volume_tag>) const
{
    const auto lMinMax = MinMax(aStartIndex, aSize, column_v<volume_tag>);

    return std::make_pair(std::logf(lMinMax.first), std::logf(lMinMax.second));
}



}    // namespace abollo