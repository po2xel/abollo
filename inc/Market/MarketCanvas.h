#ifndef __ABOLLO_MARKET_CANVAS_H__
#define __ABOLLO_MARKET_CANVAS_H__



#include <cmath>
#include <memory>

#include <skia/include/core/SkSurface.h>

#include "Market/Model/DataAnalyzer.h"
#include "Market/Painter.h"
#include "Market/Painter/AxisPainter.h"
#include "Utility/Median.h"



namespace abollo
{



class MarketCanvas final
{
private:
    constexpr static SkScalar MAX_CANDLE_COUNT{1024.f};
    constexpr static SkScalar MIN_CANDLE_COUNT{10.f};

    constexpr static SkScalar DEFAULT_CANDLE_DELTA{20.f};

    SkMatrix mTransMatrix;

    Axis<uint32_t, axis::Date> mXAxis;
    Axis<SkScalar, axis::Price> mPriceAxis;
    Axis<SkScalar, axis::Volume> mVolumeAxis;

    const uint32_t& mWidth;
    const uint32_t& mHeight;

    SkScalar mMinScaleX{1.f};
    SkScalar mMaxScaleX{1.f};

    SkScalar mDataScaleX{1.f};
    SkScalar mDataTransX{0.f};

    SkScalar mZoomScaleX{1.f};
    SkScalar mZoomTransX{0.f};

    constexpr static SkScalar mZoomScaleY{1.f};    //  Scale along Y axis is disabled.
    constexpr static SkScalar mZoomTransY{0.f};    //  Movement along Y axis is disabled.

    SkScalar mMousePosX{0.f};
    SkScalar mMousePosY{0.f};

    SkScalar mCandleWidth{0.f};
    uint32_t mSelectedCandle{0};

    uint32_t mStartSeq{0};
    uint32_t mEndSeq{0};

    std::unique_ptr<Painter> mpMarketPainter;
    std::unique_ptr<AxisPainter> mpAxisPainter;

    DataAnalyzer mDataAnalyzer;

    void Reload();

public:
    MarketCanvas(const uint32_t& aWidth, const uint32_t& aHeight);

    void Move(const SkScalar aPosX, const SkScalar aPosY)
    {
        mMousePosX = aPosX;
        mMousePosY = aPosY;
    }

    void MoveTo(const SkScalar aPosX, const SkScalar /*aPosY*/)
    {
        // mTransMatrix.postTranslate(aPosX, 0.f);    // Movement along Y axis is disabled.
        // mTransMatrix.postTranslate(aPosX, aPosY);

        mZoomTransX += aPosX;
    }

    void Zoom(const SkScalar /*aDeltaX*/, const SkScalar aDeltaY)
    {
        const auto lScaleX = std::fma(aDeltaY, 0.1f, 1.f);    // 1.f + aDeltaY / 10.f;
        const auto lDeltaX = mMousePosX * (1.f - lScaleX);

        const auto t = mTransMatrix.getScaleX() * lScaleX;
        const auto u = Median(mMinScaleX, mMaxScaleX, t);

        const auto lTemp = mZoomScaleX;
        mZoomScaleX      = Median(mMinScaleX, mMaxScaleX, mZoomScaleX * lScaleX);
        mZoomTransX      = std::fma(mZoomScaleX / lTemp, mZoomTransX, lDeltaX);

        // const auto lScaleY = 1.f + aDeltaY / 10.f;
        // const auto lDeltaY = mMousePosY * (1.f - lScaleY);

        // constexpr auto lScaleY = 1.f;    // Scale along Y axis is disabled.
        // constexpr auto lDeltaY = 0.f;

        // mZoomScaleY *= lScaleY;
        // mZoomTransY = std::fma(lScaleY, mZoomTransY, lDeltaY);

        /*
         * mTransMatrix.postTranslate(-mMousePosX, -mMousePosY)                 // Translate to the origin
         *          .postScale(1.f + aDeltaY / 10.f, 1.f + aDeltaY / 10.f)      // Scale
         *          .postTranslate(mMousePosX, mMousePosY);                     // Translate back to the pivot
         */
        // mTransMatrix.postConcat(SkMatrix::MakeAll(u / mTransMatrix.getScaleX(), 0.f, lDeltaX, 0.f, lScaleY, lDeltaY, 0.f, 0.f, 1.f));
    }

    void Capture(SkSurface* apSurface) const;
    void Paint(SkSurface* apSurface);
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_CANVAS_H__
