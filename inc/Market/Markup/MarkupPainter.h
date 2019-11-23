#ifndef __ABOLLO_MARKET_MARKUP_PAINTER_H__
#define __ABOLLO_MARKET_MARKUP_PAINTER_H__



#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkPaint.h>

#include "Market/Markup/Markup.h"



namespace abollo
{



class MarkupPainter
{
private:
    mutable SkPaint mPaint;

public:
    MarkupPainter()
    {
        mPaint.setAntiAlias(true);
        mPaint.setColor(SK_ColorMAGENTA);
        mPaint.setStyle(SkPaint::kStrokeAndFill_Style);
        mPaint.setStrokeWidth(1.5f);
    }

    void SetColor(const SkColor aColor)
    {
        mPaint.setColor(aColor);
    }

    template <typename T>
    void Draw(SkCanvas& /*aCanvas*/, T&& /*aMarkup*/) const
    {
    }

    template <typename T>
    void Highlight(SkCanvas& /*aCanvas*/, T&& /*aMarkup*/) const
    {
    }
};


template <>
inline void MarkupPainter::Draw(SkCanvas& aCanvas, const TrendLine& aMarkup) const
{
    aCanvas.drawLine(aMarkup.Start(), aMarkup.End(), mPaint);
}


template <>
inline void MarkupPainter::Highlight(SkCanvas& aCanvas, const TrendLine& aMarkup) const
{
    mPaint.setStyle(SkPaint::kStroke_Style);
    aCanvas.drawLine(aMarkup.Start(), aMarkup.End(), mPaint);

    SkPath lPath;

    // lPath.moveTo(aMarkup.Start());
    // lPath.lineTo(aMarkup.End());

    lPath.moveTo(aMarkup.Start());
    lPath.addCircle(aMarkup.Start().x(), aMarkup.Start().y(), 5.f);

    lPath.moveTo(aMarkup.End());
    lPath.addCircle(aMarkup.End().x(), aMarkup.End().y(), 5.f);

    mPaint.setStyle(SkPaint::kStroke_Style);
    aCanvas.drawPath(lPath, mPaint);

    mPaint.setColor(SK_ColorDKGRAY);
    mPaint.setStyle(SkPaint::kFill_Style);
    aCanvas.drawPath(lPath, mPaint);
}


// template <>
// inline void MarkupPainter::Draw(SkCanvas& aCanvas, const FibRetracement& aMarkup) const
// {
//     printf("Fib: %f,%f\n", aMarkup.start.x(), aMarkup.start.y());
// }



}    // namespace abollo



#endif    // __ABOLLO_MARKET_MARKUP_PAINTER_H__
