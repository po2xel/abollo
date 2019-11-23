#ifndef __ABOLLO_MARKET_MARKUP_H__
#define __ABOLLO_MARKET_MARKUP_H__



#include <variant>

#include <skia/include/core/SkPoint.h>



namespace abollo
{



inline constexpr SkScalar HIT_TEST_EPSILON{2.5f};



struct None
{
    void Begin(const SkPoint& /*aStart*/)
    {
    }

    void Next(const SkPoint& /*aEnd*/)
    {
    }

    void End(const SkPoint& /*aEnd*/)
    {
    }

    bool HitTest(const SkPoint& /*aPos*/) const
    {
        return false;
    }
};



class TrendLine
{
private:
    SkPoint mStart;
    SkPoint mEnd;

public:
    TrendLine() = default;

    [[nodiscard]] const SkPoint& Start() const
    {
        return mStart;
    }

    [[nodiscard]] const SkPoint& End() const
    {
        return mEnd;
    }

    void Begin(const SkPoint& aStart)
    {
        mStart = aStart;
    }

    void Next(const SkPoint& aEnd)
    {
        mEnd = aEnd;
    }

    void End(const SkPoint& aEnd)
    {
        mEnd = aEnd;
    }

    [[nodiscard]] bool HitTest(const SkPoint& aPos) const
    {
        // https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line
        const auto lDelta = mEnd - mStart;

        const auto lDistance = (lDelta.y() * aPos.x() - lDelta.x() * aPos.y() + mEnd.x() * mStart.y() - mEnd.y() * mStart.x()) / std::hypot(lDelta.x(), lDelta.y());

        return lDistance <= HIT_TEST_EPSILON && lDistance >= -HIT_TEST_EPSILON;
    }
};



struct FibRetracement
{
    SkPoint start;
    SkPoint end;

    FibRetracement() = default;

    void Begin(const SkPoint& aStart)
    {
        start = aStart;
    }

    void Next(const SkPoint& aEnd)
    {
        end = aEnd;
    }

    void End(const SkPoint& aEnd)
    {
        end = aEnd;
    }

    bool HitTest(const SkPoint& /*aPos*/) const
    {
        return false;
    }
};



using MarkupType = std::variant<None, TrendLine, FibRetracement>;



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MARKUP_H__
