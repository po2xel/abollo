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

    std::tie(mStartSeq, mEndSeq) = mDataAnalyzer.LoadIndex("000905.SH", 0, 1024);

    Resize();

    mpMarketPainter = std::make_unique<Painter>();
    mpAxisPainter   = std::make_unique<AxisPainter>();
    mpMarkupPainter = std::make_unique<MarkupPainter>();

    mMarkups.emplace_back();
}


void MarketCanvas::Resize()
{
    mDataScaleX = mWidth / DEFAULT_CANDLE_DELTA;
    mDataTransX = mWidth - mEndSeq / DEFAULT_CANDLE_DELTA * mWidth;    //  -offset * width / delta

    ZoomX();
    PanX();

    Reload();
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


void MarketCanvas::ZoomX()
{
    mXAxis.scale = mZoomScaleX * mDataScaleX;

    /*
     * candleCount = DEFAULT_CANDLE_DELTA / scaleX;
     * candleWidth = windowWidth / candleCount = windowWidth * scaleX / DEFAULT_CANDLE_DELTA;
     */
    // mCandleWidth = mWidth * lScaleX / DEFAULT_CANDLE_DELTA = = mZoomScaleX * mDataScaleX;
    mCandleWidth = mXAxis.scale;

    /*
     * The maximum candle index at coordinate (0, y, 1) is (endSeq - MIN_CANDLE_COUNT), which can be
     * calculated by the inversion of transformation matrix:
     *      endSeq - MIN_CANDLE_COUNT = -Tx / Sx
     * So the minimum translation along x axis is:
     *      Tx = -(endSeq - MIN_CANDLE_COUNT) * Sx = (MIN_CANDLE_COUNT - endSeq) * Sx       (1)
     * Also,
     *      Tx = zoomScaleX * dataTransX + panTransX                                        (2)
     * Replace Tx in (1) with (2):
     *      zoomScaleX * dataTransX + panTransX = (MIN_CANDLE_COUNT - endSeq) * Sx
     * the minimum pan translation is:
     *      minPanTransX = (MIN_CANDLE_COUNT - endSeq) * Sx - zoomScaleX * dataTransX
     */
    mMinPanTransX = (MIN_CANDLE_COUNT - mEndSeq) * mXAxis.scale - mZoomScaleX * mDataTransX;

    /*
     * The minimum candle index at coordinate (width, y, 1) is: (startSeq + MIN_CANDLE_COUNT), which can be
     * calculated by the inversion of transformation matrix:
     *      startSeq + MIN_CANDLE_COUNT = (width - Tx) / Sx
     * So the maximum translation along x axis is:
     *      Tx = width - (startSeq + MIN_CANDLE_COUNT) * Sx                             (1)
     * Also,
     *      Tx = zoomScaleX * dataTransX + panTransX                                    (2)
     * Replace Tx in (1) with (2):
     *      zoomScaleX * dataTransX + panTransX = width - (startSeq + MIN_CANDLE_COUNT) * Sx
     * the maximum pan translation is:
     *      maxPanTransX = width - (startSeq + MIN_CANDLE_COUNT) * Sx - zoomScaleX * dataTransX;
     */
    mMaxPanTransX = mWidth - (mStartSeq + MIN_CANDLE_COUNT) * mXAxis.scale - mZoomScaleX * mDataTransX;
}


void MarketCanvas::PanX()
{
    mXAxis.trans = std::fma(mZoomScaleX, mDataTransX, mZoomTransX);    // mZoomScaleX * mDataTransX + mZoomTransX;
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
    // mCandleWidth = mZoomScaleX * mDataScaleX;

    /*const auto lOffset = mEndSeq - DEFAULT_CANDLE_DELTA;
    mXAxis.scale       = lScaleX * mWidth / DEFAULT_CANDLE_DELTA;
    mXAxis.trans       = -lScaleX * lOffset * mWidth / DEFAULT_CANDLE_DELTA + lTransX;*/

    // mXAxis.scale = mZoomScaleX * mDataScaleX;
    // mXAxis.trans = mZoomScaleX * mDataTransX + mZoomTransX;

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

    mTransPrices = mDataAnalyzer.Saxpy<log_price_tag>(mXAxis.min, mXAxis.max, mXAxis.scale, mXAxis.trans, mPriceAxis.scale, mPriceAxis.trans, mVolumeAxis.scale, mVolumeAxis.trans);
}


void MarketCanvas::Paint(SkSurface* apSurface) const
{
    auto& lCanvas = *(apSurface->getCanvas());

    lCanvas.clear(SK_ColorDKGRAY);

    // const auto lPrice = std::expf((mMousePosY - mPriceAxis.trans) / mPriceAxis.scale);
    // fmt::print("(x, y) -> ({}, {}) -> ({}, {})\n", mMousePosX, mMousePosY, mSelectedCandle, lPrice);

    mpMarketPainter->DrawCandle(lCanvas, mTransPrices, mCandleWidth);

    mpAxisPainter->Draw<axis::Right>(lCanvas, mPriceAxis);
    mpAxisPainter->Draw<axis::Left>(lCanvas, mVolumeAxis);

    const auto& lCandleData = mDataAnalyzer[mXAxis.max - Median(mXAxis.min, mXAxis.max, mSelectedCandle)];
    mpMarketPainter->Highlight(lCanvas, lCandleData, mCandleWidth);

    {
        SkAutoCanvasRestore lGuard(&lCanvas, true);

        // lCanvas.translate(mXAxis.trans, mPriceAxis.trans);
        // lCanvas.scale(mXAxis.scale, mPriceAxis.scale);

        for (const auto& lMarkup : mMarkups)
            std::visit(
                [&lCanvas, &lPainter = *mpMarkupPainter, lPos = SkPoint::Make(mMousePosX, mMousePosY)](auto&& aMarkup) {
                    if (aMarkup.HitTest(lPos))
                    {
                        lPainter.SetColor(SK_ColorYELLOW);
                        lPainter.Highlight(lCanvas, aMarkup);
                    }
                    else
                    {
                        lPainter.SetColor(SK_ColorMAGENTA);
                        lPainter.Draw(lCanvas, aMarkup);
                    }
                },
                lMarkup);
    }
}



}    // namespace abollo