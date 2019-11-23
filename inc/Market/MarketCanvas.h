#ifndef __ABOLLO_MARKET_CANVAS_H__
#define __ABOLLO_MARKET_CANVAS_H__



#include <cmath>
#include <memory>
#include <vector>

#include <skia/include/core/SkPath.h>
#include <skia/include/core/SkSurface.h>

#include "Market/Markup/MarkupPainter.h"
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

    /*
     * 1. Transform x coordinate from data range (0, delta) to window range (0, width):
     *      dataScale = width / delta               (1)
     *
     *  The minimum scale is:
     *      dataScale_min = width / MAX_COUNT       (2)
     *  The maximum scale is:
     *      dataScale_max = width / MIN_COUNT       (3)
     *
     * 2. Zoom/Pan transformations in the window coordinate system:
     *    modelScale' = modelScale * scale_delta * dataScale
     * and
     *    dataScale_min <= modelScale' <= dataScale_max,
     * replace dataScale_min and dataScale_max with (2) and (3):
     *    width / MAX_COUNT <= modelScale * scale_delta * dataScale <= width / MIN_COUNT
     * replace dataScale with (1):
     *    width / MAX_COUNT <= modelScale * scale_delta * width / delta <= width / MIN_COUNT
     * the final result is:
     *    delta / MAX_COUNT <= modelScale * scale_delta <= delta / MIN_COUNT
     */
    constexpr static SkScalar MIN_ZOOM_SCALE_X = DEFAULT_CANDLE_DELTA / MAX_CANDLE_COUNT;    // width / MAX_CANDLE_COUNT;
    constexpr static SkScalar MAX_ZOOM_SCALE_X = DEFAULT_CANDLE_DELTA / MIN_CANDLE_COUNT;    // width / MIN_CANDLE_COUNT;

    constexpr static SkScalar mZoomScaleY{1.f};    //  Scale along Y axis is disabled.
    constexpr static SkScalar mZoomTransY{0.f};    //  Movement along Y axis is disabled.

    Axis<uint32_t, axis::Date> mXAxis;
    Axis<SkScalar, axis::Price> mPriceAxis;
    Axis<SkScalar, axis::Volume> mVolumeAxis;

    const uint32_t& mWidth;
    const uint32_t& mHeight;

    SkScalar mMinPanTransX{0.f};
    SkScalar mMaxPanTransX{0.f};

    SkScalar mDataScaleX{1.f};
    SkScalar mDataTransX{0.f};

    SkScalar mZoomScaleX{1.f};
    SkScalar mZoomTransX{0.f};

    DatePricePair mTransPrices;

    SkScalar mMousePosX{0.f};
    SkScalar mMousePosY{0.f};

    SkScalar mCandleWidth{0.f};
    uint32_t mSelectedCandle{0};

    uint32_t mStartSeq{0};
    uint32_t mEndSeq{0};

    std::unique_ptr<Painter> mpMarketPainter;
    std::unique_ptr<AxisPainter> mpAxisPainter;
    std::unique_ptr<MarkupPainter> mpMarkupPainter;

    DataAnalyzer mDataAnalyzer;

    std::vector<MarkupType> mMarkups;

    [[nodiscard]] SkPoint ConvertToData(const SkScalar aPosX, const SkScalar aPosY) const
    {
        // return {(aPosX - mXAxis.trans) / mXAxis.scale, std::expf((aPosY - mPriceAxis.trans) / mPriceAxis.scale)};
        return {(aPosX - mXAxis.trans) / mXAxis.scale, (aPosY - mPriceAxis.trans) / mPriceAxis.scale};
    }

    [[nodiscard]] SkPoint ConvertToData(const SkPoint& aPos) const
    {
        return ConvertToData(aPos.x(), aPos.y());
    }

    [[nodiscard]] SkPoint ConvertToPixel(const SkScalar aPosX, const SkScalar aPosY) const
    {
        // return {std::fma(aPosX, mXAxis.scale, mXAxis.trans), std::fma(std::logf(aPosY), mPriceAxis.scale, mPriceAxis.trans)};
        return {std::fma(aPosX, mXAxis.scale, mXAxis.trans), std::fma(aPosY, mPriceAxis.scale, mPriceAxis.trans)};
    }

    [[nodiscard]] SkPoint ConvertToPixel(const SkPoint& aPos) const
    {
        return ConvertToPixel(aPos.x(), aPos.y());
    }

    void ZoomX();
    void PanX();

    void Reload();

public:
    MarketCanvas(const uint32_t& aWidth, const uint32_t& aHeight);

    template <typename T = None, typename... Args>
    void ResetMode(Args&&... aArgs)
    {
        mMarkups.back().emplace<T>(std::forward<Args>(aArgs)...);
    }

    void Begin(const SkScalar aPosX, const SkScalar aPosY)
    {
        std::visit([aStart = SkPoint::Make(aPosX, aPosY)](auto&& aMarkup) { aMarkup.Begin(aStart); }, mMarkups.back());
    }

    void Next(const SkScalar aPosX, const SkScalar aPosY)
    {
        std::visit([aNext = SkPoint::Make(aPosX, aPosY)](auto&& aMarkup) { aMarkup.Next(aNext); }, mMarkups.back());
    }

    void End(const SkScalar aPosX, const SkScalar aPosY)
    {
        std::visit([aEnd = SkPoint::Make(aPosX, aPosY)](auto&& aMarkup) { aMarkup.End(aEnd); }, mMarkups.back());

        mMarkups.emplace_back();
    }

    // template <typename... Args>
    // void Begin(Args&&... aArgs)
    // {
    //     std::visit([... aArgs = std::forward<Args>(aArgs)](auto&& aMarkup) { aMarkup.Begin(aArgs...); }, mCurrentMarkup);
    // }

    void Pick(const SkScalar aPosX, const SkScalar aPosY)
    {
        mMousePosX = aPosX;
        mMousePosY = aPosY;

        if (const auto lCandlePosX = std::lround((mMousePosX - mXAxis.trans) / mXAxis.scale); lCandlePosX >= 0)
            mSelectedCandle = lCandlePosX;
        else
            mSelectedCandle = 0;
    }

    void Pan(const SkScalar aPosX, const SkScalar /*aPosY*/)
    {
        // mTransMatrix.postTranslate(aPosX, 0.f);    // Movement along Y axis is disabled.
        // mTransMatrix.postTranslate(aPosX, aPosY);

        mZoomTransX = Median(mMinPanTransX, mMaxPanTransX, mZoomTransX + aPosX);

        PanX();

        Reload();
    }

    void Resize();

    void Zoom(const SkScalar /*aDeltaX*/, const SkScalar aDeltaY)
    {
        const auto lScaleX = std::fma(aDeltaY, 0.1f, 1.f);    // 1.f + aDeltaY / 10.f;
        const auto lDeltaX = mMousePosX * (1.f - lScaleX);

        const auto lTemp = mZoomScaleX;
        mZoomScaleX      = Median(MIN_ZOOM_SCALE_X, MAX_ZOOM_SCALE_X, mZoomScaleX * lScaleX);

        ZoomX();

        mZoomTransX = std::fma(mZoomScaleX / lTemp, mZoomTransX, lDeltaX);
        mZoomTransX = Median(mMinPanTransX, mMaxPanTransX, mZoomTransX);

        PanX();

        Reload();

        /*const auto lTemp = mZoomScaleX;
        mZoomScaleX      = Median(MIN_ZOOM_SCALE_X, MAX_ZOOM_SCALE_X, mZoomScaleX * lScaleX);
        mZoomTransX      = std::fma(mZoomScaleX / lTemp, mZoomTransX, lDeltaX);*/

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
    void Paint(SkSurface* apSurface) const;
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_CANVAS_H__
