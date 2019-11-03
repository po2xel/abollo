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



template <typename T>
constexpr const T& median(const T& a, const T& b, const T& c)
{
    return std::max(std::min(a, b), std::min(std::max(a, b), c));
}



MarketCanvas::MarketCanvas()
{
    // using date::operator"" _y;

    // constexpr auto lStartDate{2019_y / 10 / 20}, lEndDate{2020_y / 1 / 1};
    // mDataAnalyzer.LoadIndex("000905.SH", lStartDate, lEndDate);

    std::tie(mStartSeq, mEndSeq) = mDataAnalyzer.LoadIndex("000001.SH", 0, 1024);

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


void MarketCanvas::Reload(SkCanvas& aCanvas)
{
    SkAutoCanvasRestore lGuard(&aCanvas, true);

    aCanvas.concat(mTransMatrix);

    const auto lCanvasClipBounds = aCanvas.getDeviceClipBounds();

    const auto lWidth  = static_cast<SkScalar>(lCanvasClipBounds.width());     // canvas width
    const auto lHeight = static_cast<SkScalar>(lCanvasClipBounds.height());    // canvas height
    // const auto lRangeX = 20.f;                                                 // range in x axis is: [-Inf., rangeX]

    const auto& lTransMatrix = aCanvas.getTotalMatrix();
    const auto lScaleX       = lTransMatrix.getScaleX();
    const auto lScaleY       = lTransMatrix.getScaleY();
    const auto lTransX       = lTransMatrix.getTranslateX();
    const auto lTransY       = lTransMatrix.getTranslateY();

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
    mCandleWidth = lWidth / DEFAULT_CANDLE_DELTA * lScaleX;

    const auto lOffset = mEndSeq - DEFAULT_CANDLE_DELTA;
    mXAxis.scale       = lScaleX * (lWidth / DEFAULT_CANDLE_DELTA);
    mXAxis.trans       = -lScaleX * (lOffset * (lWidth / DEFAULT_CANDLE_DELTA)) + lTransX;

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
    mXAxis.max = std::lround((lWidth - mXAxis.trans) / mXAxis.scale);

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
    mPriceAxis.scale = lScaleY * lHeight / (lLow - lHigh);
    mPriceAxis.trans = lScaleY * (lHeight * lHigh / (lHigh - lLow)) + lTransY;

    auto [lMin, lMax] = mDataAnalyzer.MinMax<log_volume_tag>(mXAxis.min, mXAxis.max);
    lMin *= 0.99f;
    lMax *= 1.01f;
    mVolumeAxis.min         = lMin;
    mVolumeAxis.max         = lMax;
    mVolumeAxis.scale       = lScaleY * lHeight / (lMin - lMax);
    mVolumeAxis.trans       = lScaleY * (lHeight * lMax / (lMax - lMin)) + lTransY;

    if (std::isinf(mVolumeAxis.scale))
        fmt::print("volume scale: {}, trans: {}\n\n", mVolumeAxis.scale, mVolumeAxis.trans);
}


void MarketCanvas::Paint(SkSurface* apSurface)
{
    auto& lCanvas = *(apSurface->getCanvas());

    lCanvas.clear(SK_ColorDKGRAY);

    Reload(lCanvas);

    const auto& lTransPrices =
        mDataAnalyzer.Saxpy<log_price_tag>(mXAxis.min, mXAxis.max, mXAxis.scale, mXAxis.trans, mPriceAxis.scale, mPriceAxis.trans, mVolumeAxis.scale, mVolumeAxis.trans);

    SkAutoCanvasRestore lGuard(&lCanvas, true);
    lCanvas.resetMatrix();

    mpMarketPainter->DrawCandle(lCanvas, lTransPrices, mCandleWidth);

    mpAxisPainter->Draw<axis::Right>(lCanvas, mPriceAxis);
    mpAxisPainter->Draw<axis::Left>(lCanvas, mVolumeAxis);

    const auto& lCandleData = mDataAnalyzer[mXAxis.max - median(mXAxis.min, mXAxis.max, mSelectedCandle)];
    mpMarketPainter->Highlight(lCanvas, lCandleData, mCandleWidth);
}



}    // namespace abollo