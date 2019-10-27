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



MarketCanvas::MarketCanvas()
{
    using date::operator"" _y;

    constexpr auto lStartDate{2018_y / 1 / 1}, lEndDate{2020_y / 1 / 1};

    mDataAnalyzer.LoadIndex("000905.SH", lStartDate, lEndDate);

    mpMarketPainter = std::make_unique<Painter>();
    mpAxisPainter = std::make_unique<AxisPainter>();
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
    const auto lRangeX = 20.f;                                                 // range in x axis is: [-Inf., rangeX]

    const auto& lTransMatrix = aCanvas.getTotalMatrix();
    const auto lScaleX       = lTransMatrix.getScaleX();
    const auto lScaleY       = lTransMatrix.getScaleY();
    const auto lTransX       = lTransMatrix.getTranslateX();
    const auto lTransY       = lTransMatrix.getTranslateY();

    mCandleWidth = lScaleX * lWidth / lRangeX;

    mXAxis.scale = -lWidth * lScaleX / lRangeX;
    mXAxis.trans = lScaleX * lWidth + lTransX;

    const auto lStartIndex = std::lround((lWidth - mXAxis.trans) / mXAxis.scale);
    if (lStartIndex >= 0 && static_cast<std::size_t>(lStartIndex) < mDataAnalyzer.Size())
        mXAxis.min = static_cast<uint32_t>(lStartIndex);
    else
        mXAxis.min = 0;

    if (const auto lCandlePosX = std::lround((mMousePosX - mXAxis.trans) / mXAxis.scale); lCandlePosX >= 0)
        mSelectedCandle = static_cast<uint32_t>(lCandlePosX);
    else
        mSelectedCandle = 0;

    const auto lEndIndex = static_cast<uint32_t>(std::ceil(lWidth / mCandleWidth + 1.f + lStartIndex));

    if (lEndIndex <= mDataAnalyzer.Size())
        mXAxis.max = lEndIndex;
    else
        mXAxis.max = static_cast<uint32_t>(mDataAnalyzer.Size());

    const auto [lLow, lHigh] = mDataAnalyzer.MinMax(mXAxis.min, mXAxis.stride(), column_v<log_price_tag>);    // range in y axis is: [low boundary, high boundary]
    mPriceAxis.min           = lLow;
    mPriceAxis.max           = lHigh;
    mPriceAxis.scale         = lScaleY * lHeight / (lLow - lHigh);
    mPriceAxis.trans         = lScaleY * (lLow * lHeight / (lHigh - lLow) + lHeight) + lTransY;

    const auto [lMin, lMax] = mDataAnalyzer.MinMax(mXAxis.min, mXAxis.stride(), column_v<log_volume_tag>);
    mVolAxis.min            = lMin;
    mVolAxis.max            = lMax;
    mVolAxis.scale          = lScaleY * lHeight / (lMin - lMax);
    mVolAxis.trans          = lScaleY * (lMin * lHeight / (lMax - lMin) + lHeight) + lTransY;
}


void MarketCanvas::Paint(SkSurface* apSurface)
{
    auto& lCanvas = *(apSurface->getCanvas());

    lCanvas.clear(SK_ColorDKGRAY);

    Reload(lCanvas);

    SkAutoCanvasRestore lGuard(&lCanvas, true);

    lCanvas.concat(mTransMatrix);

    const auto lCanvasClipBounds = lCanvas.getDeviceClipBounds();

    const auto width   = static_cast<SkScalar>(lCanvasClipBounds.width());     // canvas width
    const auto height  = static_cast<SkScalar>(lCanvasClipBounds.height());    // canvas height
    const auto lRangeX = 20.f;                                                 // range in x axis is: [-Inf., rangeX]

    const auto& lTransMat = lCanvas.getTotalMatrix();
    const auto lTransX1   = lTransMat.getTranslateX();
    const auto lScaleX1   = lTransMat.getScaleX();
    // const auto lTotalTranX = (lScaleX1 - 1.f) * width + lTransX1;
    mCandleWidth = lScaleX1 * width / lRangeX;

    mScaleX = -width * lScaleX1 / lRangeX;
    mTransX = lScaleX1 * width + lTransX1;

    const auto lStartIndex = std::lround((width - mTransX) / mScaleX);
    if (lStartIndex >= 0 && static_cast<std::size_t>(lStartIndex) < mDataAnalyzer.Size())
        mStartIndex = static_cast<uint32_t>(lStartIndex);
    else
        mStartIndex = 0;

    if (const auto lCandlePosX = std::lround((mMousePosX - mTransX) / mScaleX); lCandlePosX >= 0)
        mSelectedCandle = static_cast<uint32_t>(lCandlePosX);
    else
        mSelectedCandle = 0;

    const auto lSize = static_cast<uint32_t>(std::ceil(width / mCandleWidth + 1.f));

    if (lSize <= mDataAnalyzer.Size())
        mSize = lSize;

    // std::cout << "start index: " << mStartIndex << "\tsize: " << mSize << "\tselected: " << mSelectedCandle << std::endl;

    // auto [lLow, lHigh] = mDataAnalyzer.MinMax(mStartIndex, mSize, column_v<price_tag>); // range in y axis is: [low boundary, high boundary]
    auto [lLow, lHigh] = mDataAnalyzer.MinMax(mStartIndex, mSize, column_v<log_price_tag>);    // range in y axis is: [low boundary, high boundary]
    auto [lMin, lMax]  = mDataAnalyzer.MinMax(mStartIndex, mSize, column_v<log_volume_tag>);

    /** Calculate coordinate system transformation matrix:
     * 1. calculate y coordinate in the top-left origin system:
     *   a. invert y axis and move y origin to the bottom:
     *          | 1   0   0     |
     *    A =   | 0   -1 height |
     *          | 0   0   1     |
     *
     *   b. invert x axis and move x origin to the left:
     *          | -1   0 width  |
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
    const auto& lPriceMat = SkMatrix::MakeAll(-width / lRangeX, 0.f, width, 0.f, height / (lLow - lHigh), lLow * height / (lHigh - lLow) + height, 0.f, 0.f, 1.f);
    const auto [lScaleX, lTransX, lScaleY, lTransY] = PriceTrans(lCanvas, lPriceMat);

    const auto& lVolMat           = SkMatrix::MakeAll(width / lRangeX, 0.f, 0.f, 0.f, height / (lMin - lMax), lMin * height / (lMax - lMin) + height, 0.f, 0.f, 1.f);
    const auto [lScaleZ, lTransZ] = VolumeTrans(lCanvas, lVolMat);

    const auto& lTransPrices = mDataAnalyzer.LogSaxpy(mStartIndex, mSize, lScaleX, lTransX, lScaleY, lTransY, lScaleZ, lTransZ);

    lCanvas.resetMatrix();

    constexpr auto lYDelta = 60u;
    const auto lCount      = static_cast<uint32_t>(std::ceil(height / lYDelta));

    mpMarketPainter->DrawCandle(lCanvas, lTransPrices, mCandleWidth);

    mpAxisPainter->Draw<axis::Right>(lCanvas, mPriceAxis, lCount);
    mpAxisPainter->Draw<axis::Left>(lCanvas, mVolAxis, lCount);

    assert(mSelectedCandle >= mStartIndex);

    const auto& lSelectedCandle = mDataAnalyzer[mSelectedCandle - mStartIndex];
    mpMarketPainter->Highlight(lCanvas, lSelectedCandle, mCandleWidth);
}



}    // namespace abollo