#include "Market/Painter.h"

#include <limits>
#include <tuple>

#include <fmt/format.h>
#include <skia/include/core/SkFontMgr.h>
#include <skia/include/effects/SkDashPathEffect.h>



namespace abollo
{


Painter::Painter()
{
    mCandlePaint.setAntiAlias(true);
    mCandlePaint.setStyle(SkPaint::kFill_Style);

    mCandlestickPaint.setAntiAlias(true);
    mCandlestickPaint.setStyle(SkPaint::kStroke_Style);

    mVolumePaint.setAntiAlias(true);
    mVolumePaint.setStyle(SkPaint::kStroke_Style);
    mVolumePaint.setColor(SK_ColorBLUE);
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


void Painter::DrawCandle(SkCanvas& aCanvas, const Index& aPrices)
{
    auto lPosX = 20.f;

    SkPath lUpStickPath;
    SkPath lDownStickPath;
    SkColor lCandleColor;

    for (const auto& lPrice : aPrices)
    {
        // Draw candle stick
        const auto lLow  = static_cast<SkScalar>(lPrice.low);
        const auto lHigh = static_cast<SkScalar>(lPrice.high);

        if (lPrice.open < lPrice.close)
        {
            lCandleColor = SK_ColorRED;

            lUpStickPath.moveTo(lPosX, lLow);
            lUpStickPath.lineTo(lPosX, lHigh);
        }
        else
        {
            lCandleColor = SK_ColorGREEN;

            lDownStickPath.moveTo(lPosX, lLow);
            lDownStickPath.lineTo(lPosX, lHigh);
        }

        // Draw candle body
        const auto lDeltaX = 1.f / 2;

        const SkPoint lCandlePts[]{{lPosX - lDeltaX, static_cast<SkScalar>(lPrice.open)}, {lPosX + lDeltaX, static_cast<SkScalar>(lPrice.close)}};
        SkRect lCandleRect{};
        lCandleRect.set(lCandlePts[0], lCandlePts[1]);

        mCandlePaint.setColor(lCandleColor);

        aCanvas.drawRect(lCandleRect, mCandlePaint);

        --lPosX;
    }

    mCandlestickPaint.setColor(SK_ColorRED);
    aCanvas.drawPath(lUpStickPath, mCandlestickPaint);

    mCandlestickPaint.setColor(SK_ColorGREEN);
    aCanvas.drawPath(lDownStickPath, mCandlestickPaint);
}


void Painter::DrawVolume(SkCanvas& aCanvas, const Index& aPrices) const
{
    SkAutoCanvasRestore lGuard(&aCanvas, true);

    // auto& lTransMatrix = aCanvas.getTotalMatrix();
    // const auto lScaleX = lTransMatrix.getScaleX();
    // const auto lTransX = lTransMatrix.getTranslateX();
    // const auto lScaleY = lTransMatrix.getScaleY();
    // const auto lTransY = lTransMatrix.getTranslateY();

    // aCanvas.resetMatrix();

    auto lPosX{20.f};
    SkPath lVolumePath;

    lVolumePath.moveTo(lPosX, aPrices[0].volume);

    for (std::size_t lIndex = 1, lSize = aPrices.size(); lIndex < lSize; ++lIndex)
        lVolumePath.lineTo(--lPosX, aPrices[lIndex].volume);

    aCanvas.drawPath(lVolumePath, mVolumePaint);
}


void Painter::DrawDateAxis(SkCanvas& aCanvas, const Index& aPrices) const
{
    SkAutoCanvasRestore lGuard(&aCanvas, true);

    auto& lTransMatrix = aCanvas.getTotalMatrix();
    const auto lScaleX = lTransMatrix.getScaleX();
    const auto lTransX = lTransMatrix.getTranslateX();

    const auto lCanvasClipBounds = aCanvas.getDeviceClipBounds();
    const auto lCoordY           = static_cast<SkScalar>(lCanvasClipBounds.height());

    aCanvas.resetMatrix();

    auto lPosX     = 20;
    auto lPrevPosX = std::numeric_limits<float>::max();

    for (const auto& lPrice : aPrices)
    {
        const auto lCoordX = (lPosX--) * lScaleX + lTransX - mDateLabelWidth / 2.f;

        if (lPrevPosX - lCoordX < mDateLabelSpace)
            continue;

        const auto& lDate     = lPrice.date;
        const auto lDateLabel = fmt::format(DEFAULT_DATE_FORMAT_STR, static_cast<unsigned>(lDate.month()), static_cast<unsigned>(lDate.day()));

        aCanvas.drawString(lDateLabel.data(), lCoordX, lCoordY, mAxisLabelFont, mAxisPaint);

        lPrevPosX = lCoordX;
    }
}


void Painter::DrawPriceAxis(SkCanvas& aCanvas, const Index& aPrices) const
{
    SkAutoCanvasRestore lGuard(&aCanvas, true);

    auto& lTransMatrix = aCanvas.getTotalMatrix();
    const auto lScaleY = lTransMatrix.getScaleY();
    const auto lTransY = lTransMatrix.getTranslateY();

    const auto lCanvasClipBounds = aCanvas.getDeviceClipBounds();
    const auto lCoordX           = static_cast<SkScalar>(lCanvasClipBounds.width()) - mPriceLabelWidth;

    aCanvas.resetMatrix();

    const auto [lLow, lHigh] = aPrices.MinMax<Data::ePrice>();
    const auto lPriceDelta   = (lHigh - lLow) / DEFAULT_PRICE_LABEL_COUNT;

    auto lPrePosY = std::numeric_limits<float>::max();

    for (auto lIndex = 0; lIndex < DEFAULT_PRICE_LABEL_COUNT; ++lIndex)
    {
        const auto lPrice  = lLow + lIndex * lPriceDelta;
        const auto lCoordY = lPrice * lScaleY + lTransY;

        if (lPrePosY - lCoordY < mPriceLabelSpace)
            continue;

        auto lPriceLabel = fmt::format(DEFAULT_PRICE_FORMAT_STR, lPrice);

        aCanvas.drawString(lPriceLabel.data(), lCoordX, lCoordY, mAxisLabelFont, mAxisPaint);

        lPrePosY = lCoordY;
    }
}


// void Painter::DrawVolumeAxis(SkCanvas& aCanvas, const Index& aPrices) const
//{
//    SkAutoCanvasRestore lGuard(&aCanvas, true);
//
//    auto& lTransMatrix = aCanvas.getTotalMatrix();
//    const auto lScaleY = lTransMatrix.getScaleY();
//    const auto lTransY = lTransMatrix.getTranslateY();
//
//    // const auto lCanvasClipBounds = aCanvas.getDeviceClipBounds();
//    const auto lCoordX = 0.f; // static_cast<SkScalar>(lCanvasClipBounds.width()) - mPriceLabelWidth;
//
//    aCanvas.resetMatrix();
//
//    const auto [lLow, lHigh] = aPrices.MinMax<Data::eVolume>();
//    const auto lVolumeDelta   = (lHigh - lLow) / DEFAULT_PRICE_LABEL_COUNT;
//
//    auto lPrePosY = std::numeric_limits<float>::max();
//
//    for (auto lIndex = 0; lIndex < DEFAULT_PRICE_LABEL_COUNT; ++lIndex)
//    {
//        const auto lVolume  = lLow + lIndex * lVolumeDelta;
//        const auto lCoordY = lVolume * lScaleY + lTransY;
//
//        if (lPrePosY - lCoordY < mPriceLabelSpace)
//            continue;
//
//        auto lPriceLabel = fmt::format(DEFAULT_PRICE_FORMAT_STR, lVolume);
//
//        aCanvas.drawString(lPriceLabel.data(), lCoordX, lCoordY, mAxisLabelFont, mAxisPaint);
//
//        lPrePosY = lCoordY;
//    }
//}



}    // namespace abollo