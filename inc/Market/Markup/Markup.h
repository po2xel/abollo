#ifndef __ABOLLO_MARKET_MARKUP_H__
#define __ABOLLO_MARKET_MARKUP_H__



#include <variant>

#include <skia/include/core/SkPoint.h>



namespace abollo
{



inline bool HistTestCircle(const SkScalar aDistance)
{
    constexpr static auto HIT_TEST_EPSILON{2.5f};

    return aDistance <= HIT_TEST_EPSILON && aDistance >= -HIT_TEST_EPSILON;
}



enum class ControlPointType
{
    eNone,
    eBegin,
    eMiddle,
    eEnd
};



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

    [[nodiscard]] ControlPointType HitTest(const SkPoint& /*aPos*/) const
    {
        return ControlPointType::eNone;
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

    [[nodiscard]] ControlPointType HitTest(const SkPoint& aPos) const
    {
        auto lDistance = SkPoint::Distance(aPos, mStart);

        if (HistTestCircle(lDistance))
            return ControlPointType::eBegin;

        lDistance = SkPoint::Distance(aPos, mEnd);

        printf("%f\n", lDistance);

        if (HistTestCircle(lDistance))
            return ControlPointType::eEnd;

        // https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line
        const auto lDelta = mEnd - mStart;

        lDistance = (lDelta.y() * aPos.x() - lDelta.x() * aPos.y() + mEnd.x() * mStart.y() - mEnd.y() * mStart.x()) / std::hypot(lDelta.x(), lDelta.y());

        if (HistTestCircle(lDistance))
            return ControlPointType::eMiddle;

        return ControlPointType::eNone;
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

    static ControlPointType HitTest(const SkPoint& /*aPos*/)
    {
        return ControlPointType::eNone;
    }
};



using MarkupType = std::variant<None, TrendLine, FibRetracement>;



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MARKUP_H__
