#include "MarketingCanvas.h"

#include <skia/include/core/SkCanvas.h>



namespace abollo
{



void MarketingCanvas::Paint(SkSurface* aSurface)
{
    auto lCanvas = aSurface->getCanvas();

    lCanvas->clear(SK_ColorDKGRAY);
}



}    // namespace abollo