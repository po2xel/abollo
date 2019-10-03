#include "MarketingPainter.h"

#include <fmt/format.h>

#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkPaint.h>
#include <skia/include/core/SkPath.h>
#include <iostream>



namespace abollo
{



void MarketingPainter::Paint(SkSurface* aSurface)
{
    auto lCanvas = aSurface->getCanvas();

    lCanvas->clear(SK_ColorDKGRAY);

    lCanvas->save();

    lCanvas->concat(mTransMatrix);

    SkPaint paint;
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(8);
    paint.setColor(0xff4285F4);
    paint.setAntiAlias(true);
    paint.setStrokeCap(SkPaint::kRound_Cap);

    SkPath path;
    path.moveTo(10, 10);
    path.quadTo(256, 64, 128, 128);
    path.quadTo(10, 192, 250, 250);
    lCanvas->drawPath(path, paint);

    lCanvas->restore();
}



}    // namespace abollo