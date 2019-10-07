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

    double lMin{0}, lMax{0};
    mMinMaxStmt.exchange(into(lMin));
    mMinMaxStmt.exchange(into(lMax));

    mMinMaxStmt.define_and_bind();
    mMinMaxStmt.execute(true);
    mMinMaxStmt.bind_clean_up();

    mMinMax.first  = static_cast<float>(lMin);
    mMinMax.second = static_cast<float>(lMax);
}



}    // namespace abollo