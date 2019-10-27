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

    SkRect lPriceLabelBound{};
    mPriceLabelWidth  = mAxisLabelFont.measureText(DEFAULT_PRICE_FORMAT.data(), DEFAULT_PRICE_FORMAT.size(), SkTextEncoding::kUTF8, &lPriceLabelBound);
    mPriceLabelHeight = lPriceLabelBound.height();
    mPriceLabelSpace  = mPriceLabelHeight + mPriceLabelHeight;
}


SkScalar Painter::DrawDateAxis(SkCanvas& aCanvas, const SkScalar aCoordX, const SkScalar aCoordY, const date::year_month_day& aDate) const
{
    const auto lCoordX    = aCoordX - mDateLabelWidth / 2.f;
    const auto lDateLabel = fmt::format(DEFAULT_DATE_FORMAT_STR, static_cast<unsigned>(aDate.month()), static_cast<unsigned>(aDate.day()));

    aCanvas.drawString(lDateLabel.data(), lCoordX, aCoordY, mAxisLabelFont, mAxisPaint);

    return lCoordX;
}


void Painter::DrawPriceAxis(SkCanvas& aCanvas, const SkScalar aCoordX, const SkScalar aLow, const SkScalar aHigh, const uint32_t aCount, const SkScalar aScaleY,
                            const SkScalar aTransY) const
{
    const auto lPriceDelta = (aHigh - aLow) / aCount;
    const auto lCoordX     = aCoordX - mPriceLabelWidth;

    // auto lPrePosY = std::numeric_limits<float>::max();

    auto lPrice = aLow;

    for (uint32_t lIndex = 0; lIndex <= aCount; ++lIndex, lPrice += lPriceDelta)
    {
        // const auto lPrice  = aLow + lIndex * lPriceDelta;
        const auto lCoordY = lPrice * aScaleY + aTransY;

        // if (lPrePosY - lCoordY < mPriceLabelSpace)
        // continue;

        const auto lPriceLabel = fmt::format(DEFAULT_PRICE_FORMAT_STR, std::expf(lPrice));

        aCanvas.drawString(lPriceLabel.data(), lCoordX, lCoordY, mAxisLabelFont, mAxisPaint);

        // lPrePosY = lCoordY;
    }
}


void Painter::DrawVolumeAxis(SkCanvas& aCanvas, const SkScalar aCoordX, const SkScalar aMin, const SkScalar aMax, const uint32_t aCount, const SkScalar aScaleY,
                             const SkScalar aTransY) const
{
    const auto lVolumeDelta = (aMax - aMin) / aCount;

    // auto lPrePosY = std::numeric_limits<float>::max();

    auto lVolume = aMin;

    for (uint32_t lIndex = 0; lIndex <= aCount; ++lIndex, lVolume += lVolumeDelta)
    {
        // const auto lVolume = aMin + lIndex * lVolumeDelta;
        const auto lCoordY = lVolume * aScaleY + aTransY;

        // if (lPrePosY - lCoordY < mPriceLabelSpace)
        // continue;

        const auto lPriceLabel = fmt::format(DEFAULT_PRICE_FORMAT_STR, std::expf(lVolume));

        aCanvas.drawString(lPriceLabel.data(), aCoordX, lCoordY, mAxisLabelFont, mAxisPaint);

        // lPrePosY = lCoordY;
    }
}


void Painter::Highlight(SkCanvas& aCanvas, const MarketDataFields& aSelectedCandle, const SkScalar aCandleWidth)
{
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(0x80349a45);
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(8.f);
    paint.setStrokeCap(SkPaint::kRound_Cap);

    aCanvas.drawPoint(aSelectedCandle.index, aSelectedCandle.volume, paint);

    const auto lCandleColor = aSelectedCandle.open > aSelectedCandle.close ? DEFAULT_UPPER_COLOR : DEFAULT_LOWER_COLOR;

    const auto lHalfWidth = 1.05f * aCandleWidth / 2.f;

    // Draw candle body
    const SkPoint lCandlePts[]{{aSelectedCandle.index - lHalfWidth, aSelectedCandle.open}, {aSelectedCandle.index + lHalfWidth, aSelectedCandle.close}};
    SkRect lCandleRect{};
    lCandleRect.set(lCandlePts[0], lCandlePts[1]);

    mCandlePaint.setColor(lCandleColor);

    aCanvas.drawRect(lCandleRect, mCandlePaint);

    const auto lPriceLabel = fmt::format(DEFAULT_PRICE_FORMAT_STR, aSelectedCandle.volume);

    aCanvas.drawString(lPriceLabel.data(), aSelectedCandle.index, aSelectedCandle.volume, mAxisLabelFont, mAxisPaint);

    // aCanvas.drawString(lPriceLabel.data(), aSelectedCandle.index, aSelectedCandle.open, mAxisLabelFont, mAxisPaint);
    // aCanvas.drawString(lPriceLabel.data(), aSelectedCandle.index, aSelectedCandle.close, mAxisLabelFont, mAxisPaint);
    // aCanvas.drawString(lPriceLabel.data(), aSelectedCandle.index, aSelectedCandle.low, mAxisLabelFont, mAxisPaint);
    // aCanvas.drawString(lPriceLabel.data(), aSelectedCandle.index, aSelectedCandle.high, mAxisLabelFont, mAxisPaint);
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

    auto lBegin = lData.first;

    SkPath lVolumePath;

    {
        const auto& lPrice = thrust::get<1>(*lBegin);    // *(reinterpret_cast<const PriceWithIndex*>(&lTransPrice));
        const auto lCoordX = lPrice.index;

        lVolumePath.moveTo(lCoordX, lPrice.volume);
    }

    for (const auto lEnd = lData.second; lBegin != lEnd; ++lBegin)
    {
        const auto& lDate   = thrust::get<0>(*lBegin);
        const auto& lFields = thrust::get<1>(*lBegin);    //*(reinterpret_cast<const PriceWithIndex*>(&lTransPrice));
        const auto lCoordX  = lFields.index;

        // Draw volume curve
        lVolumePath.lineTo(lFields.index, lFields.volume);

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