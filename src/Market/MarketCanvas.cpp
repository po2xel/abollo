#include "Market/MarketCanvas.h"

#include <fstream>
#include <tuple>
#include <utility>

#include <date/date.h>
#include <fmt/format.h>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkPath.h>
#include <skia/include/core/SkTextBlob.h>

#include <include/effects/SkDashPathEffect.h>
#include "Market/Model/DataAnalyzer.h"


namespace abollo
{



MarketCanvas::MarketCanvas()
{
    using date::operator"" _y;

    constexpr auto lStartDate{2016_y / 1 / 1}, lEndDate{2020_y / 1 / 1};

    mIndexData.LoadIndex(lStartDate, lEndDate);

    mpMarketPainter = std::make_unique<Painter>();
}


void MarketCanvas::Capture(SkSurface* apSurface) const
{
    const auto lImageSnapshot = apSurface->makeImageSnapshot();
    const auto lImageData     = lImageSnapshot->encodeToData();

    std::ofstream fout("snapshot-test.png", std::ios::binary);
    fout.write(static_cast<const char*>(lImageData->data()), lImageData->size());
}


std::tuple<float, float, float, float> PriceTrans(SkCanvas& aCanvas, const SkMatrix& aTransMatrix)
{
    SkAutoCanvasRestore lGuard(&aCanvas, true);

    aCanvas.concat(aTransMatrix);

    const auto& lTransMatrix = aCanvas.getTotalMatrix();

    return {lTransMatrix.getScaleX(), lTransMatrix.getTranslateX(), lTransMatrix.getScaleY(), lTransMatrix.getTranslateY()};
}


std::pair<float, float> VolumeTrans(SkCanvas& aCanvas, const SkMatrix& aTransMatrix)
{
    SkAutoCanvasRestore lGuard(&aCanvas, true);

    aCanvas.concat(aTransMatrix);

    const auto& lTransMatrix = aCanvas.getTotalMatrix();

    return {lTransMatrix.getScaleY(), lTransMatrix.getTranslateY()};
}


void MarketCanvas::Paint(SkSurface* apSurface) const
{
    auto& lCanvas = *(apSurface->getCanvas());

    lCanvas.clear(SK_ColorDKGRAY);

    SkAutoCanvasRestore lGuard(&lCanvas, true);

    // auto p = lpCanvas->getLocalClipBounds();
    // auto p2 = lpCanvas->getDeviceClipBounds();

    // lpCanvas->clipRect(SkRect::MakeWH(1000, 700), true);
    // p  = lpCanvas->getLocalClipBounds();
    // p2 = lpCanvas->getDeviceClipBounds();

    lCanvas.concat(mTransMatrix);

    // p  = lpCanvas->getLocalClipBounds();
    // p2 = lpCanvas->getDeviceClipBounds();

    /*const SkScalar intervals[] = {10.0f, 5.0f, 2.0f, 5.0f};
    size_t count               = sizeof(intervals) / sizeof(intervals[0]);
    SkPaint paint;
    paint.setPathEffect(SkDashPathEffect::Make(intervals, (int)count, 0.0f));
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(2.0f);
    paint.setAntiAlias(true);
    lpCanvas->clear(SK_ColorWHITE);
    lpCanvas->drawLine(0, 0, 1024, 768, paint);*/

    const auto lCanvasClipBounds = lCanvas.getDeviceClipBounds();

    const auto width   = static_cast<SkScalar>(lCanvasClipBounds.width());     // canvas width
    const auto height  = static_cast<SkScalar>(lCanvasClipBounds.height());    // canvas height
    const auto lRangeX = 20.f;                                                 // range in x axis is: [-Inf., rangeX]

    const auto& lTransMat   = lCanvas.getTotalMatrix();
    const auto lTransX1     = lTransMat.getTranslateX();
    const auto lScaleX1     = lTransMat.getScaleX();
    const auto lTotalTranX  = (lScaleX1 - 1.f) * width + lTransX1;
    const auto lCandleWidth = lScaleX1 * width / lRangeX;

    const auto lStartIndex = lTotalTranX > 0.f ? static_cast<uint32_t>(lTotalTranX / lCandleWidth + 0.5f) : 0u;

    if (lStartIndex < mIndexData.Size())
        mStartIndex = lStartIndex;

    const auto lSize = static_cast<uint32_t>(std::ceil(width / lCandleWidth + 1.f));

    if (lSize <= mIndexData.Size())
        mSize = lSize;

    // std::cout << "Candle StartIndex: " << mStartIndex << std::endl;
    // std::cout << "Candle Size: " << mSize << std::endl;

    {
        SkAutoCanvasRestore lGuard2(&lCanvas, true);

        auto [lLow, lHigh, lMin, lMax] = mIndexData.MinMax(mStartIndex, mSize);    // range in y axis is: [low boundary, high boundary]

        /** Calculate coordinate system transformation matrix:
         * 1. calculate y coordinate in the top-left origin system:
         *   a. invert y axis and move y origin to the bottom:
         *          | 1   0   0     |
         *    A =   | 0   -1 height |
         *          | 0   0   1     |
         *
         *   b. invert x axis and move x origin to the left:
         *          | -1   0 width  |
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
        const auto& lPriceMat = SkMatrix::MakeAll(-width / lRangeX, 0.f, width, 0.f, height / (lLow - lHigh), lLow * height / (lHigh - lLow) + height, 0.f, 0.f, 1.f);
        const auto [lScaleX, lTransX, lScaleY, lTransY] = PriceTrans(lCanvas, lPriceMat);

        const auto& lVolMat           = SkMatrix::MakeAll(width / lRangeX, 0.f, 0.f, 0.f, height / (lMin - lMax), lMin * height / (lMax - lMin) + height, 0.f, 0.f, 1.f);
        const auto [lScaleZ, lTransZ] = VolumeTrans(lCanvas, lVolMat);

        const auto& lTransPrices = mIndexData.Saxpy(mStartIndex, mSize, lScaleX, lTransX, lScaleY, lTransY, lScaleZ, lTransZ);

        lCanvas.resetMatrix();

        constexpr auto lYDelta = 60u;
        const auto lCount      = static_cast<uint32_t>(std::ceil(height / lYDelta));

        mpMarketPainter->DrawCandle(lCanvas, lTransPrices, lCandleWidth);
        mpMarketPainter->DrawPriceAxis(lCanvas, width, lLow, lHigh, lCount, lScaleY, lTransY);
        mpMarketPainter->DrawVolumeAxis(lCanvas, 0.f, lMin, lMax, lCount, lScaleZ, lTransZ);
    }
}



}    // namespace abollo