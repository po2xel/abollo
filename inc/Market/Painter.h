#ifndef __ABOLLO_MARKET_PAINTER_H__
#define __ABOLLO_MARKET_PAINTER_H__



#include <string_view>
#include <utility>

#include <date/date.h>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkFont.h>
#include <skia/include/core/SkPaint.h>

#include "Market/Model/Column.h"



namespace abollo
{



union PriceWithIndex;


class Painter final
{
private:
    constexpr static std::string_view DEFAULT_DATE_FORMAT     = "00/00";    // The default date format is MM/DD
    constexpr static std::string_view DEFAULT_DATE_FORMAT_STR = "{:02}/{:02}";

    constexpr static std::string_view DEFAULT_PRICE_FORMAT     = "{00000.00}";
    constexpr static std::string_view DEFAULT_PRICE_FORMAT_STR = "{:>8.2f}";

    constexpr static int DEFAULT_PRICE_LABEL_COUNT = 10;

    SkPaint mCandlePaint;
    SkPaint mCandlestickPaint;
    SkPaint mAxisPaint;
    SkPaint mVolumePaint;

    SkFont mAxisLabelFont;

    SkScalar mDateLabelWidth;
    SkScalar mDateLabelSpace;

    SkScalar mPriceLabelWidth;
    SkScalar mPriceLabelHeight;
    SkScalar mPriceLabelSpace;

    SkScalar DrawDateAxis(SkCanvas& aCanvas, const SkScalar aCoordX, const SkScalar aCoordY, const date::year_month_day& aDate) const;

public:
    Painter();
    virtual ~Painter() = default;

    void Highlight(SkCanvas& aCanvas, const PriceWithIndex& aSelectedCandle, const SkScalar aCandleWidth);

    void DrawCandle(SkCanvas& aCanvas, const std::pair<DatePriceZipIterator, DatePriceZipIterator>& lData, const SkScalar aCandleWidth);

    void DrawPriceAxis(SkCanvas& aCanvas, const SkScalar aCoordX, const SkScalar aLow, const SkScalar aHigh, const uint32_t aCount, const SkScalar aScaleY,
                       const SkScalar aTransY) const;
    void DrawVolumeAxis(SkCanvas& aCanvas, const SkScalar aCoordX, const SkScalar aMin, const SkScalar aMax, const uint32_t aCount, const SkScalar aScaleY,
                        const SkScalar aTransY) const;
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_PAINTER_H__
