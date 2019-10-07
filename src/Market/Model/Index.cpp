#include "Market/Model/Index.h"

#include <algorithm>



namespace abollo
{



void Index::LoadPrices(const date::year_month_day& aStartDate, const date::year_month_day& aEndDate)
{
    using soci::into;
    using soci::use;

    Price lPrice{};

    mIndexDailyStmt.exchange(use(aStartDate, "start"));
    mIndexDailyStmt.exchange(use(aEndDate, "end"));
    mIndexDailyStmt.exchange(into(lPrice));

    mIndexDailyStmt.define_and_bind();
    mIndexDailyStmt.execute();

    mPrices.clear();
    std::copy(soci::rowset_iterator<Price>{mIndexDailyStmt, lPrice}, soci::rowset_iterator<Price>{}, std::back_inserter(mPrices));

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

    mMinMax = std::make_tuple(static_cast<float>(lMinPrice), static_cast<float>(lMaxPrice), static_cast<float>(lMinVolume), static_cast<float>(lMaxVolume),
                              static_cast<float>(lMinAmount), static_cast<float>(lMaxAmount));
}



}    // namespace abollo