#ifndef __ABOLLO_MARKET_PAINTER_H__
#define __ABOLLO_MARKET_PAINTER_H__


#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkPaint.h>
#include <skia/include/core/SkFont.h>

#include "Market/Model/Index.h"



namespace abollo
{



class Painter final
{
private:
    constexpr static std::string_view DEFAULT_DATE_FORMAT     = "00/00";    // The default date format is MM/DD
    constexpr static std::string_view DEFAULT_DATE_FORMAT_STR = "{:02}/{:02}";

    constexpr static std::string_view DEFAULT_PRICE_FORMAT = "{00000.00}";
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

public:
    Painter();
    virtual ~Painter() = default;

    void DrawCandle(SkCanvas& aCanvas, const Index& aPrices);
    void DrawVolume(SkCanvas& aCanvas, const Index& aPrices) const;
    void DrawDateAxis(SkCanvas& aCanvas, const Index& aPrices) const;
    void DrawPriceAxis(SkCanvas& aCanvas, const Index& aPrices) const;
    void DrawVolumeAxis(SkCanvas& aCanvas, const Index& aPrices) const;
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_PAINTER_H__
