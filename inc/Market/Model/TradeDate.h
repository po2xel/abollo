#ifndef __ABOLLO_MARKET_TRADE_DATE_H__
#define __ABOLLO_MARKET_TRADE_DATE_H__



#include <string_view>
#include <vector>

#include <date/date.h>

#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>



namespace soci
{


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


class TradeDate final
{
private:
    const std::string EX_SSE = "SSE";

    const std::string TRADE_DATE_SQL = "SELECT date "
                                       "FROM trade_calendar "
                                       "WHERE exchange = :exchange AND is_open = 1 "
                                       "ORDER BY date DESC "
                                       "LIMIT 20";

    soci::session mSession{soci::sqlite3, R"(data/ashare.db)"};
    soci::statement mTradeDateStmt;

    std::vector<date::year_month_day> mTradeDates{20};

public:
    TradeDate() : mTradeDateStmt(mSession.prepare << TRADE_DATE_SQL)
    {
    }

    void Load()
    {
        mTradeDateStmt.exchange(soci::use(EX_SSE, "exchange"));
        mTradeDateStmt.exchange(soci::into(mTradeDates));

        mTradeDateStmt.define_and_bind();
        mTradeDateStmt.execute(true);
        mTradeDateStmt.bind_clean_up();
    }

    auto& operator[](const std::size_t aIndex) const
    {
        return mTradeDates.at(aIndex);
    }

    [[nodiscard]] std::size_t size() const
    {
        return mTradeDates.size();
    }

    [[nodiscard]] auto& front() const
    {
        return mTradeDates.back();
    }

    [[nodiscard]] auto& back() const
    {
        return mTradeDates.front();
    }

    [[nodiscard]] auto begin() const
    {
        return mTradeDates.cbegin();
    }

    [[nodiscard]] auto end() const
    {
        return mTradeDates.cend();
    }
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_TRADE_DATE_H__
