#include "Market/Model/DataAnalyzer.h"

#include <soci/values.h>

#include "Market/Model/DataAnalyzerImpl.h"
#include "Market/Model/PagedMarketingTable.h"



namespace
{



using RowType = abollo::Row<abollo::DataAnalyzer::PagedTableType::Schema>;



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



DataAnalyzer::DataAnalyzer() : mImpl{std::make_unique<DataAnalyzerImpl<PagedTableType::Schema>>()}
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
    return mImpl->Saxpy(aStartIndex, aSize, aScaleX, aTransX, aScaleY, aTransY, aScaleZ, aTransZ);
}


PriceWithIndex DataAnalyzer::operator[](const std::size_t aIndex) const
{
    return (*mImpl)[aIndex];
}


std::size_t DataAnalyzer::Size() const
{
    return mImpl->Size();
}


std::tuple<float, float, float, float> DataAnalyzer::MinMax(const std::size_t aStartIndex, const std::size_t aSize) const
{
    return mImpl->MinMax(aStartIndex, aSize);
}


std::pair<float, float> DataAnalyzer::MinMax(const std::size_t aStartIndex, const std::size_t aSize, const ColumnTraits<price_tag> aTag) const
{
    return mImpl->MinMax(aStartIndex, aSize, aTag);
}



}    // namespace abollo