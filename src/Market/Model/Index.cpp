#include "Market/Model/Index.h"

#include <algorithm>



namespace soci
{



template <>
struct type_conversion<abollo::Index>
{
    using base_type = values;

    static void from_base(const base_type& v, indicator /*ind*/, abollo::Index& price)
    {
        price.mDate.push_back(v.get<date::year_month_day>("date"));

        price.mOpen.push_back(static_cast<float>(v.get<double>("open")));
        price.mClose.push_back(static_cast<float>(v.get<double>("close")));
        price.mLow.push_back(static_cast<float>(v.get<double>("low")));
        price.mHigh.push_back(static_cast<float>(v.get<double>("high")));
        price.mVoume.push_back(static_cast<float>(v.get<double>("volume") / 1000000.f));
        price.mAmount.push_back(static_cast<float>(v.get<double>("amount") / 1000000.f));
    }
};



}    // namespace soci


namespace abollo
{



void Index::LoadPrices(const date::year_month_day& aStartDate, const date::year_month_day& aEndDate)
{
    using soci::into;
    using soci::use;

    Price lPrice{};

    mIndexDailyStmt.exchange(use(aStartDate, "start"));
    mIndexDailyStmt.exchange(use(aEndDate, "end"));
    // mIndexDailyStmt.exchange(into(lPrice));
    mIndexDailyStmt.exchange(into(*this));

    mIndexDailyStmt.define_and_bind();
    mIndexDailyStmt.execute();

    while (mIndexDailyStmt.fetch());

    // mPrices.clear();
    // std::copy(soci::rowset_iterator<Price>{mIndexDailyStmt, lPrice}, soci::rowset_iterator<Price>{}, std::back_inserter(mPrices));

    mIndexDailyStmt.bind_clean_up();
}


void Index::LoadMinMax(const date::year_month_day& aStartDate, const date::year_month_day& aEndDate)
{
    using soci::into;
    using soci::use;

    mMinMaxStmt.exchange(use(aStartDate, "start"));
    mMinMaxStmt.exchange(use(aEndDate, "end"));

    double lMinPrice{0}, lMaxPrice{0}, lMinVolume{0}, lMaxVolume{0}, lMinAmount{0}, lMaxAmount{0};
    mMinMaxStmt.exchange(into(lMinPrice));
    mMinMaxStmt.exchange(into(lMaxPrice));
    mMinMaxStmt.exchange(into(lMinVolume));
    mMinMaxStmt.exchange(into(lMaxVolume));
    mMinMaxStmt.exchange(into(lMinAmount));
    mMinMaxStmt.exchange(into(lMaxAmount));

    mMinMaxStmt.define_and_bind();
    mMinMaxStmt.execute(true);
    mMinMaxStmt.bind_clean_up();

    mMinMax = std::make_tuple(static_cast<float>(lMinPrice), static_cast<float>(lMaxPrice), static_cast<float>(lMinVolume / 1000000.f), static_cast<float>(lMaxVolume / 1000000.f),
                              static_cast<float>(lMinAmount / 1000000.f), static_cast<float>(lMaxAmount / 1000000.f));
}



}    // namespace abollo