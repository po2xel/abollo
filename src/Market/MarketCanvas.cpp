#include "Market/MarketCanvas.h"

#include <fstream>
#include <tuple>
#include <utility>

#include <date/date.h>
#include <fmt/format.h>
#include <include/effects/SkDashPathEffect.h>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkPath.h>
#include <skia/include/core/SkTextBlob.h>

#include "Market/Model/ColumnTraits.h"
#include "Market/Model/DataAnalyzer.h"



namespace abollo
{



MarketCanvas::MarketCanvas(const uint32_t& aWidth, const uint32_t& aHeight) : mWidth{aWidth}, mHeight{aHeight}
{
    // using date::operator"" _y;

    // constexpr auto lStartDate{2019_y / 10 / 20}, lEndDate{2020_y / 1 / 1};
    // mDataAnalyzer.LoadIndex("000905.SH", lStartDate, lEndDate);

    std::tie(mStartSeq, mEndSeq) = mDataAnalyzer.LoadIndex("000001.SH", 0, 1024);

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

    mDataScaleX = mWidth / DEFAULT_CANDLE_DELTA;
    mMinScaleX  = DEFAULT_CANDLE_DELTA / MAX_CANDLE_COUNT;    // width / MAX_CANDLE_COUNT;
    mMaxScaleX  = DEFAULT_CANDLE_DELTA / MIN_CANDLE_COUNT;    // width / MIN_CANDLE_COUNT;

    mDataTransX = mWidth - mEndSeq / DEFAULT_CANDLE_DELTA * mWidth;    //  -offset * width / delta

    mpMarketPainter = std::make_unique<Painter>();
    mpAxisPainter   = std::make_unique<AxisPainter>();
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


void MarketCanvas::Reload()
{
    /** Calculate coordinate system transformation matrix:
     * 1. Transform y coordinate within the range (low, high) from the bottom-left system to the top-left window system:
     *   a). move y origin to the high:
     *          | 1   0   0       |
     *    Ty =  | 0   1  -high    |
     *          | 0   0   1       |
     *
     *   b). invert y axis:
     *          | 1   0   0     |
     *    Sy =  | 0   -1  0     |
     *          | 0   0   1     |
     *
     *   then scale along y axis:
     *          | 1             0               0      |   | 1              0               0 |
     *    Sy =  | 0   -height / (high - low)     0     | = | 0      height / (low - high)   0 |
     *          | 0             0               1      |   | 0              0               1 |
     *
     *   c). transformations along the y axis:
     *                    | 1           0                               0               |
     *    A = Sy * Ty =   | 0   height / (low - high)     high * height / (high - low)  |
     *                    | 0           0                               1               |
     *
     * 2). Transform x coordinate within the range (offset, offset + size) from the bottom-left system to the top-left window system:
        a). move x origin to the offset:
     *          | 1   0    -offset      |
     *    Tx =  | 0   1       0         |
     *          | 0   0       1         |
     *
     *  b). scale along x axis:
     *          | width / delta     0   0     |
     *    Sx =  |       0           1   0     |
     *          |       0           0   1     |
     *
     *                    | width / delta    0    -offset * width / delta   |
     *    B = Sx * Tx =   |       0          1              0               |
     *                    |       0          0              1               |
     *
     * 3). final transformation matrix:
     *                |   width / delta           0                 -offset * width / delta       |
     *   T = A * B =  |         0         height / (low - high)   high * height / (high - low)    |
     *                |         0                 0                           1                   |
     */

    /*
     * candleCount = DEFAULT_CANDLE_DELTA / scaleX;
     * candleWidth = windowWidth / candleCount = windowWidth * scaleX / DEFAULT_CANDLE_DELTA;
     */
    // mCandleWidth = mWidth * lScaleX / DEFAULT_CANDLE_DELTA;
    mCandleWidth = mZoomScaleX * mDataScaleX;

    /*const auto lOffset = mEndSeq - DEFAULT_CANDLE_DELTA;
    mXAxis.scale       = lScaleX * mWidth / DEFAULT_CANDLE_DELTA;
    mXAxis.trans       = -lScaleX * lOffset * mWidth / DEFAULT_CANDLE_DELTA + lTransX;*/

    mXAxis.scale = mZoomScaleX * mDataScaleX;
    mXAxis.trans = mZoomScaleX * mDataTransX + mZoomTransX;

    /*
     * Calculate coordinates in the model system space:
     * Final transformation matrix:
     *      | Sx    0   Tx |
     *  T = | 0     Sy  Ty |
     *      | 0     0   1  |
     *
     * 1). inversion of matrix T(https://en.wikipedia.org/wiki/Invertible_matrix):
     *            | 1 / Sx        0         -Tx / Sx  |
     *  inv(T) =  |   0         1 / Sy      -Ty / Sy  |
     *            |   0         0               1     |
     *
     * 2). window coordinate system position (width, y, 1) to the model coordinate system:
     *               | width |      | width / Sx - Tx / Sx |     | (width - Tx) / Sx |
     *  C = inv(T) * |   y   |  =   | y / Sy - Ty / Sy     |  =  |   ( y - Ty) / Sy  |
     *               |   1   |      |        1             |     |          1        |
     */
    mXAxis.max = std::lround((mWidth - mXAxis.trans) / mXAxis.scale);

    if (mXAxis.max > mEndSeq)
        mXAxis.max = mEndSeq;

    /*
     * 3). window coordinate system position (0, y, 1) to the model coordinate system:
     *               |   0   |      |    -Tx / Sx          |     |      -Tx / Sx     |
     *  C = inv(T) * |   y   |  =   | y / Sy - Ty / Sy     |  =  |   ( y - Ty) / Sy  |
     *               |   1   |      |        1             |     |          1        |
     */
    mXAxis.min = static_cast<uint32_t>(std::floor(-mXAxis.trans / mXAxis.scale - 0.5f));

    if (mXAxis.min < mStartSeq)
        mXAxis.min = mStartSeq;

    if (const auto lCandlePosX = std::lround((mMousePosX - mXAxis.trans) / mXAxis.scale); lCandlePosX >= 0)
        mSelectedCandle = lCandlePosX;
    else
        mSelectedCandle = 0;

    auto [lLow, lHigh] = mDataAnalyzer.MinMax<log_price_tag>(mXAxis.min, mXAxis.max);    // range in y axis is: [low boundary, high boundary]
    lLow *= 0.999f;
    lHigh *= 1.001f;

    mPriceAxis.min   = lLow;
    mPriceAxis.max   = lHigh;
    mPriceAxis.scale = mZoomScaleY * mHeight / (lLow - lHigh);
    mPriceAxis.trans = mZoomScaleY * (mHeight * lHigh / (lHigh - lLow)) + mZoomTransY;

    auto [lMin, lMax] = mDataAnalyzer.MinMax<log_volume_tag>(mXAxis.min, mXAxis.max);
    lMin *= 0.99f;
    lMax *= 1.01f;
    mVolumeAxis.min   = lMin;
    mVolumeAxis.max   = lMax;
    mVolumeAxis.scale = mZoomScaleY * mHeight / (lMin - lMax);
    mVolumeAxis.trans = mZoomScaleY * (mHeight * lMax / (lMax - lMin)) + mZoomTransY;

    assert(!std::isinf(mVolumeAxis.scale));
}


void MarketCanvas::Paint(SkSurface* apSurface)
{
    auto& lCanvas = *(apSurface->getCanvas());

    lCanvas.clear(SK_ColorDKGRAY);

    Reload();

    const auto& lTransPrices =
        mDataAnalyzer.Saxpy<log_price_tag>(mXAxis.min, mXAxis.max, mXAxis.scale, mXAxis.trans, mPriceAxis.scale, mPriceAxis.trans, mVolumeAxis.scale, mVolumeAxis.trans);

    mpMarketPainter->DrawCandle(lCanvas, lTransPrices, mCandleWidth);

    mpAxisPainter->Draw<axis::Right>(lCanvas, mPriceAxis);
    mpAxisPainter->Draw<axis::Left>(lCanvas, mVolumeAxis);

    const auto& lCandleData = mDataAnalyzer[mXAxis.max - Median(mXAxis.min, mXAxis.max, mSelectedCandle)];
    mpMarketPainter->Highlight(lCanvas, lCandleData, mCandleWidth);
}



}    // namespace abollo