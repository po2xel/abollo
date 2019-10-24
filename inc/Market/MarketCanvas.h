#ifndef __ABOLLO_MARKET_CANVAS_H__
#define __ABOLLO_MARKET_CANVAS_H__



#include <cmath>
#include <memory>

#include <skia/include/core/SkSurface.h>

#include "Market/Model/DataAnalyzer.h"
#include "Market/Painter.h"



namespace abollo
{



class MarketCanvas final
{
private:
    SkMatrix mTransMatrix;

    SkScalar mMousePosX{0.f};
    SkScalar mMousePosY{0.f};

    DataAnalyzer mIndexData;

    uint32_t mSelectedCandle{0};

    uint32_t mStartIndex{0};
    uint32_t mSize{20};
    float mCandleWidth;
    float mScaleX{1.f};
    float mTransX{0.f};

    std::unique_ptr<Painter> mpMarketPainter;

public:
    MarketCanvas();

    void Move(const SkScalar aPosX, const SkScalar aPosY)
    {
        mMousePosX = aPosX;
        mMousePosY = aPosY;
    }

    void MoveTo(const SkScalar aPosX, const SkScalar /*aPosY*/)
    {
        mTransMatrix.postTranslate(aPosX, 0.f);    // Movement along Y axis is disabled.
        // mTransMatrix.postTranslate(aPosX, aPosY);
    }

    void Zoom(const SkScalar /*aDeltaX*/, const SkScalar aDeltaY)
    {
        const auto lScaleX = 1.f + aDeltaY / 10.f;
        const auto lDeltaX = mMousePosX * (1.f - lScaleX);

        // const auto lScaleY = 1.f + aDeltaY / 10.f;
        // const auto lDeltaY = mMousePosY * (1.f - lScaleY);

        constexpr auto lScaleY = 1.f;    // Scale along Y axis is disabled.
        constexpr auto lDeltaY = 0.f;

        // mTransMatrix.postTranslate(-mMousePosX, -mMousePosY)                    // Translate to the origin
        //             .postScale(1.f + aDeltaY / 10.f, 1.f + aDeltaY / 10.f)      // Scale
        //             .postTranslate(mMousePosX, mMousePosY);                     // Translate back to the pivot

        mTransMatrix.postConcat(SkMatrix::MakeAll(lScaleX, 0.f, lDeltaX, 0.f, lScaleY, lDeltaY, 0.f, 0.f, 1.f));
    }

    void Capture(SkSurface* apSurface) const;
    void Paint(SkSurface* apSurface);
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_CANVAS_H__
