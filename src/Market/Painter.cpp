#include "Market/Painter.h"

#include <limits>

#include <fmt/format.h>
#include <skia/include/core/SkFontMgr.h>
#include <skia/include/effects/SkDashPathEffect.h>

#include "Market/Model/DataAnalyzer.h"



namespace abollo
{



constexpr auto DEFAULT_UPPER_COLOR  = SkColorSetARGB(0xFF, 0xDC, 0x32, 0x2F);
constexpr auto DEFAULT_LOWER_COLOR  = SkColorSetARGB(0xFF, 0x3C, 0xc8, 0x66);
constexpr auto DEFAULT_VOLUME_COLOR = SkColorSetARGB(0xFF, 0x6C, 0x71, 0xC4);


Painter::Painter()
{
    mCandlePaint.setAntiAlias(true);
    mCandlePaint.setStyle(SkPaint::kFill_Style);

    mCandlestickPaint.setAntiAlias(true);
    mCandlestickPaint.setStyle(SkPaint::kStroke_Style);

    mVolumePaint.setAntiAlias(true);
    mVolumePaint.setStyle(SkPaint::kStroke_Style);
    mVolumePaint.setColor(DEFAULT_VOLUME_COLOR);
    mVolumePaint.setAlphaf(0.5f);

    mAxisPaint.setAntiAlias(true);
    mAxisPaint.setColor(SK_ColorWHITE);

    const char* lFontFamily = nullptr;    // Default system family, if it exists.
    const SkFontStyle lFontStyle;         // Default is normal weight, normal width,  upright slant.
    const auto lFontManager = SkFontMgr::RefDefault();
    const auto lTypeface    = lFontManager->legacyMakeTypeface(lFontFamily, lFontStyle);

    mAxisLabelFont.setTypeface(lTypeface);
    mAxisLabelFont.setSize(12.f);
    mAxisLabelFont.setScaleX(1.0f);
    mAxisLabelFont.setSkewX(0.f);
    mAxisLabelFont.setEdging(SkFont::Edging::kAntiAlias);

    mDateLabelWidth = mAxisLabelFont.measureText(DEFAULT_DATE_FORMAT.data(), DEFAULT_DATE_FORMAT.size(), SkTextEncoding::kUTF8, nullptr);
    mDateLabelSpace = mDateLabelWidth * 1.5f;
}


SkScalar Painter::DrawDateAxis(SkCanvas& aCanvas, const SkScalar aCoordX, const SkScalar aCoordY, const date::year_month_day& aDate) const
{
    const auto lCoordX    = aCoordX - mDateLabelWidth / 2.f;
    const auto lDateLabel = fmt::format(DEFAULT_DATE_FORMAT_STR, static_cast<unsigned>(aDate.month()), static_cast<unsigned>(aDate.day()));

    aCanvas.drawString(lDateLabel.data(), lCoordX, aCoordY, mAxisLabelFont, mAxisPaint);

    return lCoordX;
}


void Painter::Highlight(SkCanvas& aCanvas, const MarketDataFields& aCandleData, const SkScalar aCandleWidth)
{
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(0x80349a45);
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(8.f);
    paint.setStrokeCap(SkPaint::kRound_Cap);

    const auto lTransformed = aCandleData.transformed;
    const auto lOriginal    = aCandleData.original;

    aCanvas.drawPoint(lTransformed.seq, lTransformed.volume, paint);

    const auto lCandleColor = lTransformed.open > lTransformed.close ? DEFAULT_UPPER_COLOR : DEFAULT_LOWER_COLOR;

    const auto lHalfWidth = 1.05f * aCandleWidth / 2.f;

    // Draw candle body
    const SkPoint lCandlePts[]{{lTransformed.seq - lHalfWidth, lTransformed.open}, {lTransformed.seq + lHalfWidth, lTransformed.close}};
    SkRect lCandleRect{};
    lCandleRect.set(lCandlePts[0], lCandlePts[1]);

    mCandlePaint.setColor(lCandleColor);

    aCanvas.drawRect(lCandleRect, mCandlePaint);

    constexpr static std::string_view DEFAULT_LABEL_FORMAT_STR = "{:>8.2f}";
    auto lPriceLabel                                           = fmt::format(DEFAULT_LABEL_FORMAT_STR, lOriginal.volume);

    aCanvas.drawString(lPriceLabel.data(), lTransformed.seq, lTransformed.volume, mAxisLabelFont, mAxisPaint);

    /*lPriceLabel = fmt::format(DEFAULT_LABEL_FORMAT_STR, lOriginal.open);
    aCanvas.drawString(lPriceLabel.data(), lTransformed.seq, lTransformed.open, mAxisLabelFont, mAxisPaint);

    lPriceLabel = fmt::format(DEFAULT_LABEL_FORMAT_STR, lOriginal.close);
    aCanvas.drawString(lPriceLabel.data(), lTransformed.seq, lTransformed.close, mAxisLabelFont, mAxisPaint);

    lPriceLabel = fmt::format(DEFAULT_LABEL_FORMAT_STR, lOriginal.low);
    aCanvas.drawString(lPriceLabel.data(), lTransformed.seq, lTransformed.low, mAxisLabelFont, mAxisPaint);

    lPriceLabel = fmt::format(DEFAULT_LABEL_FORMAT_STR, lOriginal.high);
    aCanvas.drawString(lPriceLabel.data(), lTransformed.seq, lTransformed.high, mAxisLabelFont, mAxisPaint);*/
}


void Painter::DrawCandle(SkCanvas& aCanvas, const std::pair<DatePriceZipIterator, DatePriceZipIterator>& lData, const SkScalar aCandleWidth)
{
    auto lPrevCoordX = std::numeric_limits<float>::max();

    const auto lCanvasClipBounds = aCanvas.getDeviceClipBounds();
    const auto lDateCoordY       = static_cast<SkScalar>(lCanvasClipBounds.height());

    const auto lHalfWidth = aCandleWidth / 2.f;

    SkPath lUpperShadowPath;
    SkPath lLowerShadowPath;

    SkColor lCandleColor;

    auto [lBegin, lEnd] = lData;

    SkPath lVolumePath;

    {
        const auto& lPrice = thrust::get<1>(*lBegin).transformed;
        const auto lCoordX = lPrice.seq;

        lVolumePath.moveTo(lCoordX, lPrice.volume);
    }

    for (; lBegin != lEnd; ++lBegin)
    {
        const auto& lDate   = thrust::get<0>(*lBegin);
        const auto& lFields = thrust::get<1>(*lBegin).transformed;
        const auto lCoordX  = lFields.seq;

        // Draw volume curve
        lVolumePath.lineTo(lFields.seq, lFields.volume);

        // Draw date label
        if (lPrevCoordX - lCoordX >= mDateLabelSpace)
            lPrevCoordX = DrawDateAxis(aCanvas, lCoordX, lDateCoordY, lDate);

        // Draw candle stick
        {
            // Draw candle shadow
            const auto lLow  = lFields.low;
            const auto lHigh = lFields.high;

            // The transformed price is in the window coordinate system and the Y axis points down in the window coordinate system,
            // so the comparison is inverted.
            if (lFields.open > lFields.close)
            {
                lCandleColor = DEFAULT_UPPER_COLOR;

                lUpperShadowPath.moveTo(lCoordX, lLow);
                lUpperShadowPath.lineTo(lCoordX, lHigh);
            }
            else
            {
                lCandleColor = DEFAULT_LOWER_COLOR;

                lLowerShadowPath.moveTo(lCoordX, lLow);
                lLowerShadowPath.lineTo(lCoordX, lHigh);
            }

            // Draw candle body
            const SkPoint lCandlePts[]{{lCoordX - lHalfWidth, lFields.open}, {lCoordX + lHalfWidth, lFields.close}};
            SkRect lCandleRect{};
            lCandleRect.set(lCandlePts[0], lCandlePts[1]);

            mCandlePaint.setColor(lCandleColor);

            aCanvas.drawRect(lCandleRect, mCandlePaint);
        }
    }

    mCandlestickPaint.setColor(DEFAULT_UPPER_COLOR);
    aCanvas.drawPath(lUpperShadowPath, mCandlestickPaint);

    mCandlestickPaint.setColor(DEFAULT_LOWER_COLOR);
    aCanvas.drawPath(lLowerShadowPath, mCandlestickPaint);

    aCanvas.drawPath(lVolumePath, mVolumePaint);
}



}    // namespace abollo