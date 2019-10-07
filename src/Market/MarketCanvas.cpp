#include "Market/MarketCanvas.h"

#include <fmt/format.h>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkPath.h>
#include <skia/include/core/SkTextBlob.h>

#include "Market/Model/Index.h"
#include <include/effects/SkDashPathEffect.h>


namespace abollo
{



MarketCanvas::MarketCanvas()
{
    mTradeDate.Load();
    mIndexData.Load(mTradeDate.front(), mTradeDate.back());

    mpMarketPainter = std::make_unique<Painter>();
}


void MarketCanvas::Paint(SkSurface* apSurface) const
{
    auto lpCanvas = apSurface->getCanvas();

    lpCanvas->clear(SK_ColorDKGRAY);

    SkAutoCanvasRestore lGuard(lpCanvas, true);

    // auto p = lpCanvas->getLocalClipBounds();
    // auto p2 = lpCanvas->getDeviceClipBounds();

    // lpCanvas->clipRect(SkRect::MakeWH(1000, 700), true);
    // p  = lpCanvas->getLocalClipBounds();
    // p2 = lpCanvas->getDeviceClipBounds();

    lpCanvas->concat(mTransMatrix);

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

    const auto lCanvasClipBounds = lpCanvas->getDeviceClipBounds();

    const auto width   = lCanvasClipBounds.width();     // canvas width
    const auto height  = lCanvasClipBounds.height();    // canvas height
    const auto lRangeX = 20.f;                          // range in x axis is: [-Inf., rangeX]

    {
        SkAutoCanvasRestore lGuard2(lpCanvas, true);

        auto [low, high] = mIndexData.MinMax<Data::ePrice>();    // range in y axis is: [low boundary, high boundary]

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
        const auto& mat = SkMatrix::MakeAll(width / lRangeX, 0.f, 0.f, 0.f, height / (low - high), low * height / (high - low) + height, 0.f, 0.f, 1.f);

        lpCanvas->concat(mat);

        mpMarketPainter->DrawCandle(*lpCanvas, mIndexData);
        mpMarketPainter->DrawDateAxis(*lpCanvas, mIndexData);
        mpMarketPainter->DrawPriceAxis(*lpCanvas, mIndexData);
    }

    {
        SkAutoCanvasRestore lGuard2(lpCanvas, true);

        auto [low, high] = mIndexData.MinMax<Data::eVolume>();
        const auto& mat  = SkMatrix::MakeAll(width / lRangeX, 0.f, 0.f, 0.f, height / (low - high), low * height / (high - low) + height, 0.f, 0.f, 1.f);

        lpCanvas->concat(mat);

        // mpMarketPainter->DrawVolumeAxis(*lpCanvas, mIndexData);
        mpMarketPainter->DrawVolume(*lpCanvas, mIndexData);
    }
}



}    // namespace abollo