#ifndef __ABOLLO_MARKET_PAINTER_H__
#define __ABOLLO_MARKET_PAINTER_H__



#include <string_view>
#include <utility>

#include <date/date.h>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkFont.h>
#include <skia/include/core/SkPaint.h>

#include "Market/Model/MarketDataFields.h"



namespace abollo
{



class Painter final
{
private:
    constexpr static std::string_view DEFAULT_DATE_FORMAT     = "00/00";    // The default date format is MM/DD
    constexpr static std::string_view DEFAULT_DATE_FORMAT_STR = "{:02}/{:02}";

    SkPaint mCandlePaint;
    SkPaint mCandlestickPaint;
    SkPaint mAxisPaint;
    SkPaint mVolumePaint;

    SkFont mAxisLabelFont;

    SkScalar mDateLabelWidth;
    SkScalar mDateLabelSpace;

    SkScalar DrawDateAxis(SkCanvas& aCanvas, const SkScalar aCoordX, const SkScalar aCoordY, const date::year_month_day& aDate) const;

public:
    Painter();
    virtual ~Painter() = default;

    void Highlight(SkCanvas& aCanvas, const MarketDataFields& aCandleData, const SkScalar aCandleWidth);

    void DrawCandle(SkCanvas& aCanvas, const std::pair<DatePriceZipIterator, DatePriceZipIterator>& lData, const SkScalar aCandleWidth);
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_PAINTER_H__
