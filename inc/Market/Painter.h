#ifndef __ABOLLO_MARKET_PAINTER_H__
#define __ABOLLO_MARKET_PAINTER_H__


#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkPaint.h>
#include <skia/include/core/SkFont.h>

#include <include/core/SkFontMgr.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

#include "Market/Model/Index.h"



namespace abollo
{



class Painter
{
private:
    SkPaint mCandlePaint;
    SkPaint mCandlestickPaint;

    SkPaint mAxisPaint;
    SkFont mAxisLabelFont;

public:
    Painter();
    virtual ~Painter() = default;

    void Draw(SkCanvas& aCanvas, const Index& aPrices);
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_PAINTER_H__
