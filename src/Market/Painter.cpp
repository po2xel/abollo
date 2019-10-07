#include "Market/Painter.h"



namespace abollo
{


Painter::Painter()
{
    mCandlePaint.setAntiAlias(true);
    mCandlePaint.setStyle(SkPaint::kFill_Style);
    mCandlestickPaint.setAntiAlias(true);

    const char* fontFamily = nullptr;    // Default system family, if it exists.
    const SkFontStyle fontStyle;         // Default is normal weight, normal width,  upright slant.
    const auto fontManager = SkFontMgr::RefDefault();
    const auto typeface    = fontManager->legacyMakeTypeface(fontFamily, fontStyle);

    mAxisLabelFont.setTypeface(typeface);
    mAxisLabelFont.setSize(12.f);
    mAxisLabelFont.setScaleX(1.0f);
    mAxisLabelFont.setSkewX(0.f);
    mAxisLabelFont.setEdging(SkFont::Edging::kAntiAlias);

    mAxisPaint.setAntiAlias(true);
    mAxisPaint.setColor(SK_ColorWHITE);
}


void Painter::Draw(SkCanvas& aCanvas, const Index& aPrices)
{
    // const auto lDelta = 1024.f / mPrices.size();
    auto lIndex        = 20;
    const auto width   = 1024.f;    // canvas width
    const auto height  = 768.f;     // canvas height
    const auto lRangeX = 20.f;      // range in x axis is: [-Inf., rangeX]

    auto [low, high] = aPrices.MinMax();    // range in y axis is: [low boundary, high boundary]

    /** Calculate coordinate system transformation matrix:
     * 1. calculate y coordinate in the top-left origin system:  invert y axis:
     *          | 1   0   0     |
     *    A =   | 0   -1 height |
     *          | 0   0   1     |
     *
     * 2. scale along x axis is: width / delta;
     *    scale along y axis is: height / (high - low);
     *    translation along y axis is: -low * (height / (high - low));
     *    transformation matrix is:
     *          | width / delta     0                               0                   |
     *    B =   | 0             height/(high - low)     -low * (height / (high - low))  |
     *          | 0                 0                               1                   |
     *
     * 3. final transformation matrix:
     *    C = A * B
     */
    const auto mat = SkMatrix::MakeAll(width / lRangeX, 0.f, 0.f, 0.f, height / (low - high), low * height / (high - low) + height, 0.f, 0.f, 1.f);

    // SkAutoCanvasRestore lGuard(&aCanvas, true);
    aCanvas.concat(mat);

    for (const auto& lPrice : aPrices)
    {
        // Draw candle stick
        const auto lPosX = static_cast<SkScalar>(lIndex--);
        const auto lLow  = static_cast<SkScalar>(lPrice.low);
        const auto lHigh = static_cast<SkScalar>(lPrice.high);

        mCandlestickPaint.setColor(lPrice.color);

        aCanvas.drawLine(lPosX, lLow, lPosX, lHigh, mCandlestickPaint);

        // Draw candle body
        const auto lDeltaX = 1.f / 2;

        const SkPoint lCandlePts[]{{lPosX - lDeltaX, static_cast<SkScalar>(lPrice.open)}, {lPosX + lDeltaX, static_cast<SkScalar>(lPrice.close)}};
        SkRect lCandleRect{};
        lCandleRect.set(lCandlePts[0], lCandlePts[1]);

        mCandlePaint.setColor(lPrice.color);

        aCanvas.drawRect(lCandleRect, mCandlePaint);
    }
}


}    // namespace abollo